#include "spdlog.h"
#include "details/null_mutex.h"
#include "details/os.h"
#include "details/registry.h"
#include "sinks/ansicolor_sink.h"
#include "sinks/basic_file_sink.h"
#include "sinks/daily_file_sink.h"
#include "sinks/dist_sink.h"
#include "sinks/msvc_sink.h"
#include "sinks/null_sink.h"
#include "sinks/ostream_sink.h"
#include "sinks/rotating_file_sink.h"
#include "sinks/stdout_color_sinks.h"
#include "sinks/stdout_sinks.h"
#include "sinks/wincolor_sink.h"
#include "common.h"
#include "fmt/fmt.h"
#include "fmt/bundled/ostream.h"