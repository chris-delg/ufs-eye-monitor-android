#include <jni.h>
#include <stdlib.h>
#include <string.h>

int run_lsufs(int argc, char* argv[]);

JNIEXPORT jint JNICALL
Java_com_qualcomm_eyemonitorinterface_MainActivity_runLsufs(
    JNIEnv *env,
    jobject thisObject,
    jobject jargs)
{
    int argc = (*env)->GetArrayLength(env, jargs);
    char **argv = malloc(argc * sizeof(char *));

    for (int i = 0; i < argc; i++) {
        jstring jstr = (jstring) (*env)->GetObjectArrayElement(env, jargs, i);
        const char *str = (*env)->GetStringUTFChars(env, jstr, 0);
        argv[i] = strdup(str);
        (*env)->ReleaseStringUTFChars(env, jstr, str);
    }

    int result = run_lsufs(argc, argv);

    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);

    return result;
}
