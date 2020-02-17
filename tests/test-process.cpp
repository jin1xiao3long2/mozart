/**
 * Mozart++ Template Library
 * Licensed under MIT License
 * Copyright (c) 2020 Covariant Institute
 * Website: https://covariant.cn/
 * Github:  https://github.com/covariant-institute/
 */

#include <cstdio>
#include <cstdlib>
#include <mozart++/system/process.hpp>

#ifdef MOZART_PLATFORM_WIN32
#define SHELL "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe"
#else
#define SHELL "/bin/bash"
#endif

using mpp::process;
using mpp::process_builder;

void test_basic() {
    process p = process::exec(SHELL);
    p.in() << "ls /" << std::endl;
    p.in() << "exit" << std::endl;
    p.wait_for();

    std::string s;
    while (std::getline(p.out(), s)) {
        printf("process: test-basic: %s\n", s.c_str());
    }
}

void test_stderr() {
    // PS> echo fuckms 1>&2
    // + CategoryInfo          : ParserError: (:) [], ParentContainsErrorRecordException
    // + FullyQualifiedErrorId : RedirectionNotSupported

#ifndef MOZART_PLATFORM_WIN32
    // the following code is equivalent to "/bin/bash 2>&1"
    process p = process_builder().command(SHELL)
        .merge_outputs(true)
        .start();

    // write to stderr
    p.in() << "echo fuckcpp 1>&2" << std::endl;
    p.in() << "exit" << std::endl;
    p.wait_for();

    // and we can get from stdout
    std::string s;
    p.out() >> s;

    if (s != "fuckcpp") {
        printf("process: test-stderr: failed\n");
        exit(1);
    }
#endif
}

void test_env() {
    process p = process_builder().command(SHELL)
        .environment("VAR1", "fuck")
        .environment("VAR2", "cpp")
        .start();

#ifdef MOZART_PLATFORM_WIN32
    p.in() << "echo $env:VAR1" << std::endl;
#else
    p.in() << "echo $VAR1$VAR2" << std::endl;
#endif
    p.in() << "exit" << std::endl;
    p.wait_for();

    std::string s;
    p.out() >> s;

    if (s != "fuckcpp") {
        printf("process: test-env: failed\n");
        exit(1);
    }
}

void test_r_file() {
    // VAR=fuckcpp bash <<< "echo $VAR; exit" > output-all.txt

    FILE *fout = fopen("output-all.txt", "w");

    process p = process_builder().command(SHELL)
#ifdef MOZART_PLATFORM_WIN32
        .arguments(std::vector<std::string>{"-NonInteractive"})
#endif
        .environment("VAR", "fuckcpp")
        .redirect_stdout(fileno(fout))
        .merge_outputs(true)
        .start();

#ifdef MOZART_PLATFORM_WIN32
    p.in() << "echo $env:VAR" << std::endl;
#else
    p.in() << "echo $VAR" << std::endl;
#endif
    p.in() << "exit" << std::endl;
    p.wait_for();

    fclose(fout);

    fout = fopen("output-all.txt", "r");
    mpp::fdistream fin(fileno(fout));
    std::string s;
    fin >> s;

    if (s != "fuckcpp") {
        printf("process: test-redirect-file: failed\n");
        exit(1);
    }
}

void test_exit_code() {
    process p = process::exec(SHELL);

    p.in() << "exit 120" << std::endl;
    int code = p.wait_for();

    if (code != 120) {
        printf("process: test-exit-code: failed\n");
        exit(1);
    }
}

int main(int argc, const char **argv) {
    test_basic();
    test_stderr();
    test_env();
    test_r_file();
    test_exit_code();
    return 0;
}
