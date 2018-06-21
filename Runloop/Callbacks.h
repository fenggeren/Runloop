#pragma once

#include <functional>

 

namespace Callback
{

    using Functor = std::function<void()>;
    using TimerCallback = std::function<void()>;
};





