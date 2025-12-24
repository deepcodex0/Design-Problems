#include <iostream>
#include <string>
#include <chrono>
using namespace std;

/**
 * TokenBucket implements a simple token-bucket rate limiter.
 *
 * Parameters:
 *  - capacity: maximum number of tokens the bucket can hold.
 *  - tokens: current token count (fractional values allowed to enable smooth refill).
 *  - refillRate: number of tokens added per minute.
 *  - lastRefillTime: epoch seconds when the bucket was last refilled.
 *
 * Usage:
 *  - Call `isAllowable()` to attempt consuming one token; it returns `true` and consumes
 *    a token when available, otherwise returns `false`.
 *  - Tokens are refilled linearly based on the time elapsed since `lastRefillTime`.
 *
 * Notes / Caveats:
 *  - This implementation is NOT thread-safe. Add external synchronization for concurrent use.
 *  - Time resolution is in seconds; for higher precision consider using milliseconds.
 */
class TokenBucket{
    private:
        const int capacity;
        double tokens;
        int refillRate;
        int lastRefillTime;
    public:
        TokenBucket(int capacity, int refillRate) : capacity(capacity), tokens(capacity), refillRate(refillRate), lastRefillTime(0) {
            // Constructor implementation
        }

        bool isAllowable() {
            long now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            
            long timePassed = now - lastRefillTime;
            double earned = timePassed * (refillRate/60);

            tokens = std::min((double)capacity, tokens+earned);

            if(tokens >= 1.0) {
                tokens -= 1.0;
                lastRefillTime = now;
                return true;
            }

            return false;
        }
};