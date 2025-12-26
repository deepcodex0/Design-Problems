#include <cstdint>   // int64_t
#include <algorithm> // std::max
using namespace std;

/**
 * LeakyBucket implements a simple leaky-bucket rate limiter.
 *
 * Description:
 *  - The leaky bucket accepts incoming requests (conceptually "water") and
 *    processes them at a fixed, steady rate (the leak rate). When the bucket
 *    is full, additional incoming requests are dropped.
 *
 * Behavior:
 *  - `capacity` is the maximum amount the bucket can hold. Each request adds
 *    one unit of "water" when accepted.
 *  - The bucket leaks continuously at `leakRate` units per second, making room
 *    for new requests over time.
 *  - Call `allow()` to attempt to enqueue/accept a request. It returns `true`
 *    if the request is accepted (space available), otherwise `false`.
 *
 * Use cases:
 *  - Use LeakyBucket to smooth out bursts and guarantee a steady output rate.
 *  - If you want to allow bursts up to capacity and refill based on time,
 *    consider `TokenBucket` instead.
 *
 * Notes:
 *  - Time resolution here is milliseconds.
 *  - This implementation is NOT thread-safe. Add synchronization for concurrent use.
 */
class LeakyBucket {
  private:
    const int capacity; // total capacity of the bucket
    int leakRate;
    double bucket;
    int64_t last_ts;


  public:
    LeakyBucket(int capacity, int leakRate): capacity(capacity), leakRate(leakRate), last_ts(-1), bucket(0) {

    }

    bool allow(int64_t ts_ms)
    {
        // First call: initialize time anchor, no draining.
        if (last_ts == -1)
        {
            last_ts = ts_ms;
            if (bucket + 1.0 <= capacity)
            {
                bucket += 1.0;
                return true;
            }
            return false;
        }

        // Compute time delta (clamp if out-of-order).
        int64_t delta_ms = ts_ms - last_ts;
        if (delta_ms < 0)
            delta_ms = 0;

        // Drain based on leak rate (requests per second) and ms timestamps.
        double drain = delta_ms * (static_cast<double>(leakRate) / 1000.0);
        bucket = std::max(0.0, bucket - drain);

        // Update last timestamp on every call.
        last_ts = ts_ms;

        // Try to add this request.
        if (bucket + 1.0 <= capacity)
        {
            bucket += 1.0;
            return true;
        }
        return false;
    }
};