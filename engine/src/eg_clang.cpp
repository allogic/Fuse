#pragma warning(disable : 4146)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)
#pragma warning(disable : 4275)
#pragma warning(disable : 4291)
#pragma warning(disable : 4805)
#pragma warning(disable : 4996)

#include <engine/eg_pch.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#include <clang/CodeGen/CodeGenAction.h>

#include <clang/Lex/PreprocessorOptions.h>

#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>

#include <llvm/IR/Module.h>

#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>

#include <llvm/TargetParser/Host.h>

#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

#include <llvm/IR/LLVMContext.h>

#include <llvm/IRReader/IRReader.h>

#include <llvm/Support/CodeGen.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>

static std::shared_ptr<clang::DiagnosticOptions> s_compiler_diagnostic_options = {};
static std::shared_ptr<clang::TextDiagnosticPrinter> s_compiler_diagnostic_printer = {};
static std::shared_ptr<clang::DiagnosticIDs> s_compiler_diagnostic_ids = {};
static std::shared_ptr<clang::DiagnosticsEngine> s_compiler_diagnostics_engine = {};
static std::shared_ptr<clang::CompilerInvocation> s_compiler_invocation = {};
static std::shared_ptr<clang::PCHContainerOperations> s_compiler_pch_operations = {};
static std::shared_ptr<clang::CompilerInstance> s_compiler_instance = {};
static std::shared_ptr<clang::TargetOptions> s_compiler_target_options = {};

static clang::FileManager *s_compiler_file_manager = 0;
static clang::SourceManager *s_compiler_source_manager = 0;

void eg_clang_create(void) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  s_compiler_diagnostic_options = std::make_shared<clang::DiagnosticOptions>();
  s_compiler_diagnostic_printer = std::make_shared<clang::TextDiagnosticPrinter>(llvm::errs(), *s_compiler_diagnostic_options);
  s_compiler_diagnostic_ids = std::make_shared<clang::DiagnosticIDs>();
  s_compiler_diagnostics_engine = std::make_shared<clang::DiagnosticsEngine>(s_compiler_diagnostic_ids.get(), *s_compiler_diagnostic_options.get(), s_compiler_diagnostic_printer.get(), true);
  s_compiler_invocation = std::make_shared<clang::CompilerInvocation>();
  s_compiler_pch_operations = std::make_shared<clang::PCHContainerOperations>();

  char const *compiler_args[] = {
    "-std=c++20",
    "-xc++",
    "main.cpp",
  };

  clang::CompilerInvocation::CreateFromArgs(*s_compiler_invocation.get(), compiler_args, *s_compiler_diagnostics_engine.get());

  s_compiler_instance = std::make_shared<clang::CompilerInstance>(s_compiler_invocation, s_compiler_pch_operations);

  s_compiler_instance->setDiagnostics(s_compiler_diagnostics_engine.get()); // TODO: is this needed..

  s_compiler_target_options = std::make_shared<clang::TargetOptions>();

  s_compiler_target_options->Triple = llvm::sys::getDefaultTargetTriple();

  s_compiler_instance->setTarget(clang::TargetInfo::CreateTargetInfo(*s_compiler_diagnostics_engine.get(), *s_compiler_target_options.get()));

  // clang::CodeGenOptions &CGO = CI.getCodeGenOpts();
  // CGO.RelocationModel = llvm::Reloc::PIC_;
  // CGO.CodeModel = "default";

  s_compiler_file_manager = s_compiler_instance->createFileManager();

  s_compiler_instance->createSourceManager(*s_compiler_file_manager);

  s_compiler_source_manager = &s_compiler_instance->getSourceManager();

  s_compiler_instance->getInvocation().getPreprocessorOpts().RetainRemappedFileBuffers = true;
}
void eg_clang_compile(char const *source_code) {
  std::unique_ptr<llvm::MemoryBuffer> memory_buffer = llvm::MemoryBuffer::getMemBufferCopy(source_code, "main.cpp");

  s_compiler_instance->getInvocation().getPreprocessorOpts().addRemappedFile("main.cpp", memory_buffer.get());

  clang::FileEntry const *file_entry = s_compiler_file_manager->getVirtualFile("main.cpp", memory_buffer->getBufferSize(), 0);

  clang::Expected<clang::FileEntryRef> file_entry_ref = s_compiler_file_manager->getFileRef("main.cpp");

  clang::FileID file_id = s_compiler_source_manager->createFileID(*file_entry_ref, clang::SourceLocation(), clang::SrcMgr::C_User);

  s_compiler_source_manager->setMainFileID(file_id);

  s_compiler_instance->createPreprocessor(clang::TU_Complete);
  s_compiler_instance->createASTContext();

  clang::EmitLLVMOnlyAction emit_llvm_only_action;

  if (!s_compiler_instance->ExecuteAction(emit_llvm_only_action)) {
    llvm::errs() << "Error executing action\n";
  }

  std::unique_ptr<llvm::Module> module = emit_llvm_only_action.takeModule();

  if (module) {
    module->print(llvm::outs(), 0);
  }

  llvm::Expected<llvm::orc::JITTargetMachineBuilder> target_machine_builder = llvm::orc::JITTargetMachineBuilder::detectHost();

  llvm::orc::LLJITBuilder jit_builder;

  jit_builder.setJITTargetMachineBuilder(target_machine_builder.get());
  jit_builder.setDataLayout(std::make_optional(module->getDataLayout()));

  std::unique_ptr<llvm::orc::LLJIT> jit = llvm::cantFail(jit_builder.create());

  llvm::orc::ThreadSafeModule thread_safe_module(std::move(module), std::make_unique<llvm::LLVMContext>());

  llvm::cantFail(jit->addIRModule(std::move(thread_safe_module)));

  llvm::Expected<llvm::orc::ExecutorAddr> symbol = llvm::cantFail(jit->lookup("add"));

  using AddProc = int (*)(int, int);

  AddProc add_proc = symbol->toPtr<AddProc>();

  int result = add_proc(100, 42);
}
void eg_clang_destroy(void) {
}
