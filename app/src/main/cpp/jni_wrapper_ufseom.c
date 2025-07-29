// SPDX-License-Identifier: BSD-3-Clause-Clear
/*
 * Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

#ifdef ANDROID_BUILD
#include <android/log.h>
#define LOG_TAG "UFSEOM_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...) printf(__VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#endif

// Forward declaration of the main function (renamed from original main)
int run_ufseom(int argc, char *argv[]);

// JNI function to run ufseom with command line arguments
JNIEXPORT jint JNICALL
Java_com_qualcomm_eyemonitorinterface_MainActivity_runUFSEOM(JNIEnv *env, jclass clazz, jobjectArray args)
{
    int argc = 0;
    char **argv = NULL;
    jint result = -1;
    int i;

    if (args == NULL)
    {
        LOGE("Arguments array is null\n");
        return -1;
    }

    // Get the number of arguments
    argc = (*env)->GetArrayLength(env, args);
    if (argc <= 0)
    {
        LOGE("No arguments provided\n");
        return -1;
    }

    // Allocate memory for argv array
    argv = (char **)malloc(sizeof(char *) * (argc + 1));
    if (argv == NULL)
    {
        LOGE("Failed to allocate memory for argv\n");
        return -1;
    }

    // Convert Java strings to C strings
    for (i = 0; i < argc; i++)
    {
        jstring jstr = (jstring)(*env)->GetObjectArrayElement(env, args, i);
        if (jstr == NULL)
        {
            LOGE("Argument %d is null\n", i);
            goto cleanup;
        }

        const char *str = (*env)->GetStringUTFChars(env, jstr, NULL);
        if (str == NULL)
        {
            LOGE("Failed to get UTF chars for argument %d\n", i);
            (*env)->DeleteLocalRef(env, jstr);
            goto cleanup;
        }

        // Allocate memory and copy the string
        argv[i] = (char *)malloc(strlen(str) + 1);
        if (argv[i] == NULL)
        {
            LOGE("Failed to allocate memory for argument %d\n", i);
            (*env)->ReleaseStringUTFChars(env, jstr, str);
            (*env)->DeleteLocalRef(env, jstr);
            goto cleanup;
        }
        strcpy(argv[i], str);

        // Release the Java string
        (*env)->ReleaseStringUTFChars(env, jstr, str);
        (*env)->DeleteLocalRef(env, jstr);
    }
    argv[argc] = NULL;

    LOGI("Calling run_ufseom with %d arguments\n", argc);

    // Call the main function
    result = run_ufseom(argc, argv);

    LOGI("run_ufseom returned: %d\n", result);

    cleanup:
    // Free allocated memory
    if (argv != NULL)
    {
        for (i = 0; i < argc; i++)
        {
            if (argv[i] != NULL)
            {
                free(argv[i]);
            }
        }
        free(argv);
    }

    return result;
}

// Alternative JNI function with simpler signature for basic commands
JNIEXPORT jint JNICALL
Java_com_qualcomm_eyemonitorinterface_MainActivity_runUFSEOMCommand(JNIEnv *env, jclass clazz, jstring command)
{
    const char *cmd_str;
    char *argv_buffer;
    char **argv;
    int argc = 0;
    jint result = -1;
    char *token;
    int i;

    if (command == NULL)
    {
        LOGE("Command string is null\n");
        return -1;
    }

    cmd_str = (*env)->GetStringUTFChars(env, command, NULL);
    if (cmd_str == NULL)
    {
        LOGE("Failed to get UTF chars for command\n");
        return -1;
    }

    // Create a copy of the command string for tokenization
    argv_buffer = (char *)malloc(strlen(cmd_str) + 1);
    if (argv_buffer == NULL)
    {
        LOGE("Failed to allocate memory for command buffer\n");
        (*env)->ReleaseStringUTFChars(env, command, cmd_str);
        return -1;
    }
    strcpy(argv_buffer, cmd_str);

    // Count arguments
    char *temp_buffer = (char *)malloc(strlen(cmd_str) + 1);
    strcpy(temp_buffer, cmd_str);
    token = strtok(temp_buffer, " ");
    while (token != NULL)
    {
        argc++;
        token = strtok(NULL, " ");
    }
    free(temp_buffer);

    if (argc == 0)
    {
        LOGE("No arguments found in command\n");
        free(argv_buffer);
        (*env)->ReleaseStringUTFChars(env, command, cmd_str);
        return -1;
    }

    // Allocate argv array
    argv = (char **)malloc(sizeof(char *) * (argc + 1));
    if (argv == NULL)
    {
        LOGE("Failed to allocate memory for argv\n");
        free(argv_buffer);
        (*env)->ReleaseStringUTFChars(env, command, cmd_str);
        return -1;
    }

    // Tokenize the command string
    i = 0;
    token = strtok(argv_buffer, " ");
    while (token != NULL && i < argc)
    {
        argv[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    LOGI("Calling run_ufseom with command: %s\n", cmd_str);

    // Call the main function
    result = run_ufseom(argc, argv);

    LOGI("run_ufseom returned: %d\n", result);

    // Cleanup
    free(argv);
    free(argv_buffer);
    (*env)->ReleaseStringUTFChars(env, command, cmd_str);

    return result;
}
