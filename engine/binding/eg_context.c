#include <engine/eg_pch.h>
#include <engine/eg_context.h>

#include <jni.h>

JNIEXPORT jlong JNICALL Java_createContext(JNIEnv *env, jobject obj, jint id, jdouble value, jstring name) {

  // const char *cname = (*env)->GetStringUTFChars(env, name, 0);
  // MyStruct *ptr = create_struct(id, value, cname);
  // (*env)->ReleaseStringUTFChars(env, name, cname);

  return (jlong)(uintptr_t)0;
}

JNIEXPORT void JNICALL Java_updateContext(JNIEnv *env, jobject obj, jlong ptrLong, jint id, jdouble value, jstring name) {

  // MyStruct *ptr = (MyStruct *)(uintptr_t)ptrLong;
  // const char *cname = (*env)->GetStringUTFChars(env, name, 0);
  // update_struct(ptr, id, value, cname);
  // (*env)->ReleaseStringUTFChars(env, name, cname);
}

JNIEXPORT void JNICALL Java_destroyContext(JNIEnv *env, jobject obj, jlong ptrLong) {

  // MyStruct *ptr = (MyStruct *)(uintptr_t)ptrLong;
  // free_struct(ptr);
}
