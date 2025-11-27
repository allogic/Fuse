// #include <compiler/cp_pch.h>

#include <memory>
#include <vector>
#include <string>

#pragma warning(disable : 4146)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)
#pragma warning(disable : 4275)
#pragma warning(disable : 4291)
#pragma warning(disable : 4805)
#pragma warning(disable : 4996)

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

// #include "llvm/ExecutionEngine/SectionMemoryManager.h"
// #include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"

void compiler_create_module(std::string const &source_code);

int32_t main(int32_t argc, char **argv) {
  std::string source_code = R"cpp(
        extern "C" int add(int a, int b) { return a + b; }
    )cpp";

  compiler_create_module(source_code);
}

void compiler_create_module(std::string const &source_code) {
  const char *Args[] = {
    "-std=c++20",
    "-xc++",
    "input.cpp" // virtual name
  };

  auto DiagOpts = std::make_shared<clang::DiagnosticOptions>();
  auto *DiagClient = new clang::TextDiagnosticPrinter(llvm::errs(), *DiagOpts);
  auto *DiagIDs = new clang::DiagnosticIDs();

  auto Diagnostics = new clang::DiagnosticsEngine(
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(DiagIDs),
    *DiagOpts,
    DiagClient,
    /*ShouldOwnClient=*/true);

  // 2. Create invocation using diagnostics
  auto Invocation = std::make_shared<clang::CompilerInvocation>();
  clang::CompilerInvocation::CreateFromArgs(*Invocation, Args, *Diagnostics);

  // 3. Create CompilerInstance using invocation + PCH operations
  auto PCHOps = std::make_shared<clang::PCHContainerOperations>();
  clang::CompilerInstance CI(Invocation, PCHOps);

  // 4. Attach diagnostics to CI
  CI.setDiagnostics(Diagnostics);

  //////////////////////////////////////////////////////

  auto TO = std::make_shared<clang::TargetOptions>();
  TO->Triple = llvm::sys::getDefaultTargetTriple();

  CI.setTarget(clang::TargetInfo::CreateTargetInfo(CI.getDiagnostics(), *TO));

  clang::CodeGenOptions &CGO = CI.getCodeGenOpts();
  CGO.RelocationModel = llvm::Reloc::PIC_;
  CGO.CodeModel = "default";

  //////////////////////////////////////////////////////

  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());

  //////////////////////////////////////////////////////

  clang::FileManager &FM = CI.getFileManager();
  clang::SourceManager &SM = CI.getSourceManager();

  // Create a unique_ptr owner for the memory buffer
  std::unique_ptr<llvm::MemoryBuffer> buffer = llvm::MemoryBuffer::getMemBufferCopy(source_code, "input.cpp");

  // 1. Tell Clang to use this buffer when "input.cpp" is opened
  CI.getInvocation().getPreprocessorOpts().RetainRemappedFileBuffers = true;
  CI.getInvocation().getPreprocessorOpts().addRemappedFile("input.cpp", buffer.get());

  // 2. Create a virtual file entry **in FM**
  const clang::FileEntry *FE =
    FM.getVirtualFile("input.cpp", buffer->getBufferSize(), 0);

  // 3. Now FileManager CAN provide a FileEntryRef
  clang::Expected<clang::FileEntryRef> FERef = FM.getFileRef("input.cpp");
  assert(FERef && "Virtual file was not created");

  clang::FileID fileID = SM.createFileID(
    *FERef,
    clang::SourceLocation(),
    clang::SrcMgr::C_User);

  // 5. Make it the main file
  SM.setMainFileID(fileID);

  CI.createPreprocessor(clang::TU_Complete);
  CI.createASTContext();

  clang::EmitLLVMOnlyAction Act;
  if (!CI.ExecuteAction(Act)) {
    llvm::errs() << "Error executing action\n";
  }

  // -----------------------------------------
  // 8) Retrieve the LLVM module
  // -----------------------------------------
  std::unique_ptr<llvm::Module> module = Act.takeModule();

  if (module) {
    llvm::outs() << "Generated module:\n";
    module->print(llvm::outs(), nullptr);
  }

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  // --------------------------
  // 2. Create an LLJIT instance
  // --------------------------
  // llvm::TargetOptions TO;

  //  llvm::ExecutionEngine *JIT = llvm::EngineBuilder(std::move(module))
  //                                 .setEngineKind(llvm::EngineKind::JIT)
  //                                 .setOptLevel(llvm::CodeGenOptLevel::None)
  //                                 .setTargetOptions(TO)
  //                                 .setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>())
  //                                 .create();

  auto TMB = llvm::orc::JITTargetMachineBuilder::detectHost();

  // auto JTMB = llvm::cantFail(llvm::orc::JITTargetMachineBuilder::fromTargetTriple(module->getDataLayout()));

  llvm::orc::LLJITBuilder Builder;
  Builder.setJITTargetMachineBuilder(*TMB);
  Builder.setDataLayout(std::make_optional(module->getDataLayout()));

  std::unique_ptr<llvm::orc::LLJIT> JIT = llvm::cantFail(Builder.create());

  // Wrap module for thread-safe access
  llvm::orc::ThreadSafeModule TSM(std::move(module), std::make_unique<llvm::LLVMContext>());

  // --------------------------
  // 4. Add module to the JIT
  // --------------------------
  llvm::cantFail(JIT->addIRModule(std::move(TSM)));

  // --------------------------
  // 5. Look up the compiled function
  // --------------------------
  auto sym = llvm::cantFail(JIT->lookup("add"));

  // Convert JIT target address function pointer
  using AddProc = int (*)(int, int);
  // AddProc add_proc = (AddProc)sym.getAddress();
  AddProc add_proc = sym.toPtr<AddProc>();

  // --------------------------
  // 6. Execute!
  // --------------------------
  int result = add_proc(100, 42);
}
