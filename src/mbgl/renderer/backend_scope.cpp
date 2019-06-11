#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/thread_local.hpp>

#include <cassert>

namespace {

#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    MB_THREAD_LOCAL mbgl::gfx::BackendScope* backendScope;
#else
mbgl::util::ThreadLocal<mbgl::gfx::BackendScope>& currentScope() {
    static mbgl::util::ThreadLocal<mbgl::gfx::BackendScope> backendScope;

    return backendScope;
}
#endif

} // namespace

namespace mbgl {
namespace gfx {

BackendScope::BackendScope(RendererBackend& backend_, ScopeType scopeType_)
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    : priorScope(backendScope),
#else
    : priorScope(currentScope().get()),
#endif

      nextScope(nullptr),
      backend(backend_),
      scopeType(scopeType_) {
    if (priorScope) {
        assert(priorScope->nextScope == nullptr);
        priorScope->nextScope = this;
        priorScope->deactivate();
    }

    activate();

#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    backendScope = this;
#else
    currentScope().set(this);
#endif
}

BackendScope::~BackendScope() {
    assert(nextScope == nullptr);
    deactivate();

    if (priorScope) {
        priorScope->activate();
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
        backendScope = priorScope;
#else
        currentScope().set(priorScope);
#endif
        assert(priorScope->nextScope == this);
        priorScope->nextScope = nullptr;
    } else {
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
        backendScope = nullptr;
#else
        currentScope().set(nullptr);
#endif
    }
}

void BackendScope::activate() {
    if (scopeType == ScopeType::Explicit &&
            !(priorScope && this->backend == priorScope->backend) &&
            !(nextScope && this->backend == nextScope->backend)) {
        // Only activate when set to Explicit and
        // only once per RenderBackend
        backend.activate();
        activated = true;
    }
}

void BackendScope::deactivate() {
    if (activated &&
        !(nextScope && this->backend == nextScope->backend)) {
        // Only deactivate when set to Explicit and
        // only once per RenderBackend
        backend.deactivate();
        activated = false;
    }
}

bool BackendScope::exists() {
#ifdef MB_COMPILER_CXX_THREAD_LOCAL
    return backendScope;
#else
    return currentScope().get();
#endif
}

} // namespace gfx
} // namespace mbgl
