/*
 * Copyright 2010 Rockchip Electronics S.LSI Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define MODULE_TAG "mpp_buffer_test"

#include <string.h>

#include "mpp_log.h"
#include "mpp_buffer.h"

#define MPP_BUFFER_TEST_SIZE            (SZ_1K*4)
#define MPP_BUFFER_TEST_COMMIT_COUNT    10
#define MPP_BUFFER_TEST_NORMAL_COUNT    10

int main()
{
    MPP_RET ret = MPP_OK;
    MppBufferCommit commit;
    MppBufferGroup group = NULL;
    MppBuffer commit_buffer[MPP_BUFFER_TEST_COMMIT_COUNT];
    void *commit_ptr[MPP_BUFFER_TEST_COMMIT_COUNT];
    MppBuffer normal_buffer[MPP_BUFFER_TEST_NORMAL_COUNT];
    size_t size = MPP_BUFFER_TEST_SIZE;
    RK_S32 i;

    mpp_log("mpp_buffer_test start\n");

    memset(commit_ptr,    0, sizeof(commit_ptr));
    memset(commit_buffer, 0, sizeof(commit_buffer));
    memset(normal_buffer, 0, sizeof(normal_buffer));

    ret = mpp_buffer_group_get(&group, MPP_BUFFER_TYPE_NORMAL);
    if (MPP_OK != ret) {
        mpp_err("mpp_buffer_test mpp_buffer_group_get failed\n");
        goto MPP_BUFFER_failed;
    }

    mpp_log("mpp_buffer_test commit mode start\n");

    commit.type = MPP_BUFFER_TYPE_NORMAL;
    commit.size = size;

    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        commit_ptr[i] = malloc(size);
        if (NULL == commit_ptr[i]) {
            mpp_err("mpp_buffer_test malloc failed\n");
            goto MPP_BUFFER_failed;
        }

        commit.data.ptr = commit_ptr[i];

        ret = mpp_buffer_commit(group, &commit);
        if (MPP_OK != ret) {
            mpp_err("mpp_buffer_test mpp_buffer_commit failed\n");
            goto MPP_BUFFER_failed;
        }
    }

    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        ret = mpp_buffer_get(group, &commit_buffer[i], size);
        if (MPP_OK != ret) {
            mpp_err("mpp_buffer_test mpp_buffer_get commit mode failed\n");
            goto MPP_BUFFER_failed;
        }
    }

    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        ret = mpp_buffer_put(&commit_buffer[i]);
        if (MPP_OK != ret) {
            mpp_err("mpp_buffer_test mpp_buffer_put commit mode failed\n");
            goto MPP_BUFFER_failed;
        }
    }

    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        if (commit_ptr[i]) {
            free(commit_ptr[i]);
            commit_ptr[i] = NULL;
        }
    }

    mpp_log("mpp_buffer_test commit mode success\n");

    mpp_log("mpp_buffer_test normal mode start\n");

    for (i = 0; i < MPP_BUFFER_TEST_NORMAL_COUNT; i++) {
        ret = mpp_buffer_get(group, &normal_buffer[i], (i+1)*SZ_1K);
        if (MPP_OK != ret) {
            mpp_err("mpp_buffer_test mpp_buffer_get mode normal failed\n");
            goto MPP_BUFFER_failed;
        }
    }

    for (i = 0; i < MPP_BUFFER_TEST_NORMAL_COUNT; i++) {
        ret = mpp_buffer_put(&normal_buffer[i]);
        if (MPP_OK != ret) {
            mpp_err("mpp_buffer_test mpp_buffer_get mode normal failed\n");
            goto MPP_BUFFER_failed;
        }
    }

    mpp_log("mpp_buffer_test normal mode success\n");

    if (group)
        mpp_buffer_group_put(&group);

    mpp_log("mpp_buffer_test success\n");
    return ret;

MPP_BUFFER_failed:
    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        mpp_buffer_put(&commit_buffer[i]);
    }

    for (i = 0; i < MPP_BUFFER_TEST_COMMIT_COUNT; i++) {
        if (commit_ptr[i]) {
            free(commit_ptr[i]);
            commit_ptr[i] = NULL;
        }
    }

    if (group)
        mpp_buffer_group_put(group);

    mpp_log("mpp_buffer_test failed\n");
    return ret;
}
