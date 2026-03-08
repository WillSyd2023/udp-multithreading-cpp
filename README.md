# Real-Time Multi-Threaded C++ Volatility Analytics Engine

![](docs/udp_multithreading_demo.gif)

A multi-threaded system for calculating real-time volatility across parallel, sharded asset streams.

## Design
```mermaid
graph LR
    P[C++ Mock Data Generator] -- UDP via Socket --> I[Main Thread]
    subgraph "Analytics Engine"
        subgraph "Each Updates Volatility"
            W1[Worker Thread 1]
            W2[Worker Thread 2]
            W3[Worker Thread 3]
            W4[Worker Thread 4]
        end
        I -- Sharding --> W1
        I -- Sharding --> W2
        I -- Sharding --> W3
        I -- Sharding --> W4
        R[Terminal UI: Reads Shards Every 1 Second]
        W1 --> R
        W2 --> R
        W3 --> R
        W4 --> R
    end
```

## Performance
Based on the main ingestor loop of the analytics engine. 

- Average **latency** per update: 5257.34ns
- **Throughtput**, updates per second: 191799/s

## Features
- **UDP Ingestion:** Handles market data updates via sockets.
- **Parallel Processing:** Assets are sharded across worker threads.
- **Efficient Concurrency:** Uses a `std::swap` double-buffering technique to minimise time spent holding mutex locks.
- **Welford's Algorithm:** O(1) online calculation of volatility i.e. sample standard deviation for log-returns.

