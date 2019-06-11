#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_pool.hpp>

#include <mbgl/util/thread_local.hpp>

namespace mbgl {

#ifdef MB_COMPILER_CXX_THREAD_LOCAL
namespace {
    MB_THREAD_LOCAL Scheduler* currentScheduler;
} // namespace
#else
util::ThreadLocal<Scheduler> currentScheduler;
static auto& current() {
    static util::ThreadLocal<Scheduler> scheduler;
    return scheduler;
};
#endif

void Scheduler::SetCurrent(Scheduler* scheduler) {
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    currentScheduler = scheduler;
#else
    current().set(scheduler);
#endif
}

Scheduler* Scheduler::GetCurrent() {
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    return currentScheduler;
#else
    return current().get();
#endif
}

// static
std::shared_ptr<Scheduler> Scheduler::GetBackground() {
    static std::weak_ptr<Scheduler> weak;
    static std::mutex mtx;

    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<Scheduler> scheduler = weak.lock();

    if (!scheduler) {
        weak = scheduler = std::make_shared<ThreadPool>(4);
    }

    return scheduler;
}

} //namespace mbgl
