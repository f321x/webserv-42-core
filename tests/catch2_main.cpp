#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    // Create a Catch2 session
    Catch::Session session;

    // You can add any global setup code here if necessary

    // Run the tests
    int result = session.run(argc, argv);

    // Global clean-up code here if necessary

    // Return the test result code (0 if successful)
    return result;
}