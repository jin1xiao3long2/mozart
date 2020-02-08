/**
 * Mozart++ Template Library
 * Licensed under MIT License
 * Copyright (c) 2020 Covariant Institute
 * Website: https://covariant.cn/
 * Github:  https://github.com/covariant-institute/
 */
#pragma once

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace mpp {
#ifdef _WIN32
    using fd_type = HANDLE;
    static constexpr fd_type FD_INVALID = nullptr;

    ssize_t read(fd_type handle, void *buf, size_t count) {
        DWORD dwRead;
        if (ReadFile(handle, buf, count, &dwRead, nullptr)) {
            return dwRead;
        } else {
            return 0;
        }
    }

    ssize_t write(fd_type handle, const void *buf, size_t count) {
        DWORD dwWritten;
        if (WriteFile(handle, buf, count, &dwWritten, nullptr)) {
            return dwWritten;
        } else {
            return 0;
        }
    }

#else
    using fd_type = int;
    static constexpr fd_type FD_INVALID = -1;
    using ::read;
    using ::write;
#endif

    void close_fd(fd_type &fd) {
        if (fd == FD_INVALID) {
            return;
        }
#ifdef _WIN32
        CloseHandle(fd);
#else
        ::close(fd);
#endif
        fd = FD_INVALID;
    }
}
