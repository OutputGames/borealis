#if !defined(UTIL_H)
#define UTIL_H

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <coroutine>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


#include "borealis/util/io.h"

namespace brl
{
    struct UtilCoroutine
    {
        struct promise_type
        {
            float yield_value_ = 0.0f; // For yielding a delay
            UtilCoroutine get_return_object() { return UtilCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)}; }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}

            // Handle co_yield
            std::suspend_always yield_value(float value)
            {
                yield_value_ = value;
                return {};
            }

        };

        std::coroutine_handle<promise_type> handle_;
        float current_delay_ = 0.0f;

        explicit UtilCoroutine(std::coroutine_handle<promise_type> h) : handle_(h) {}

        // Delete copy operations
        UtilCoroutine(const UtilCoroutine&) = delete;
        UtilCoroutine& operator=(const UtilCoroutine&) = delete;

        // Implement move operations
        UtilCoroutine(UtilCoroutine&& other) noexcept : handle_(other.handle_), current_delay_(other.current_delay_)
        {
            other.handle_ = nullptr; // Take ownership
        }

        UtilCoroutine& operator=(UtilCoroutine&& other) noexcept
        {
            if (this != &other)
            {
                if (handle_)
                {
                    handle_.destroy();
                }
                handle_ = other.handle_;
                current_delay_ = other.current_delay_;
                other.handle_ = nullptr;
            }
            return *this;
        }

        ~UtilCoroutine()
        {
            if (handle_)
            {
                handle_.destroy();
            }
        }

        bool is_done() const { return handle_.done(); }
        void resume(float delta_time)
        {
            if (!is_done())
            {
                current_delay_ -= delta_time;
                if (current_delay_ <= 0.0f)
                {
                    handle_.resume();
                    current_delay_ = handle_.promise().yield_value_; // Update delay for next yield
                }
            }
        }

        static void startCoroutine(std::function<UtilCoroutine()> c);
    };
}

#endif // UTIL_H
