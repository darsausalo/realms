#include <daScript/daScript.h>
#include <daScript/misc/sysos.h>
#include <daScript/simulate/fs_file_info.h>
#include <doctest/doctest.h>
#include <fmt/core.h>

#include <io.h>

TEST_CASE("daScript: test1") {
    NEED_MODULE(Module_BuiltIn)
    NEED_MODULE(Module_Math);
    NEED_MODULE(Module_Strings);
    NEED_MODULE(Module_Random);

    das::setDasRoot(MOTOR_DASCRIPT_ROOT_DIR);

    auto scriptFilename =
            fmt::format("{}/hello_world.das", MOTOR_TEST_SCRIPT_DIR);

    das::CodeOfPolicies policies{};
    das::TextPrinter tout;
    das::ModuleGroup mainGroup;
    auto access = das::make_smart<das::FsFileAccess>();
    if (auto program = das::compileDaScript(scriptFilename, access, tout,
                                            mainGroup, false, policies)) {
        if (program->failed()) {
            FAIL_CHECK("failed to compile");
            for (auto& err : program->errors) {
                FAIL_CHECK(std::data(reportError(err.at, err.what, err.extra,
                                                 err.fixme, err.cerr)));
            }
        } else {
            das::Context ctx(program->getContextStackSize());
            if (!program->simulate(ctx, tout)) {

                FAIL_CHECK("failed to simulate");
                for (auto& err : program->errors) {
                    FAIL_CHECK(std::data(reportError(
                            err.at, err.what, err.extra, err.fixme, err.cerr)));
                }
            } else {
                if (auto fnMain = ctx.findFunction("main")) {
                    das::StackAllocator sharedStack(8 * 1024);
                    das::SharedStackGuard guard(ctx, sharedStack);
                    bool result;
                    if (das::verifyCall<bool>(fnMain->debugInfo, mainGroup)) {
                        auto result =
                                das::cast<bool>::to(ctx.eval(fnMain, nullptr));
                        if (!result) {
                            FAIL_CHECK(std::data(scriptFilename) << " failed");
                        }
                    }
                    if (auto ex = ctx.getException()) {
                        FAIL_CHECK(std::data(scriptFilename) << ": " << ex);
                    }
                }
            }
        }
    }

    das::Module::Shutdown();
}
