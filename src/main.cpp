#include "cli/repl.h"
#include "utils/logging.h"
#include "utils/config.h"

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    logging::initialize();
    config::initialize_from_env();

    cli::Repl repl;
    repl.run();
    return 0;
}





