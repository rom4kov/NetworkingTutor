#include <stdio.h>

// Define the ranks as an enum
typedef enum {
    RANK_PACKET_NOVICE = 0,
    RANK_LAYER_SHIKOMI = 18,
    RANK_ADDRESSINFO_DESHI = 39,
    RANK_SOCKET_APPRENTICE = 68,
    RANK_HTTP_BUSHI = 267,
    RANK_PARSER_NINJA = 400,
    RANK_ROUTER_ROMIN = 600,
    RANK_PROTOCOL_KENSEI = 800,
    RANK_ENCRYPTION_SHOGUN = 1000,
    RANK_MULTITHREAD_DAIMYO = 1300,
    RANK_STREAM_SENSEI = 1600,
    RANK_NETWORK_TENGU = 2000
} Rank;

// Function to get rank name as a string for display in ncurses
const char* get_rank_name(Rank rank) {
    switch (rank) {
        case RANK_PACKET_NOVICE: return "Packet Novice";
        case RANK_LAYER_SHIKOMI: return "Layer Shikomi";
        case RANK_ADDRESSINFO_DESHI: return "Addressinfo Deshi";
        case RANK_SOCKET_APPRENTICE: return "Socket Apprentice";
        case RANK_HTTP_BUSHI: return "HTTP Bushi";
        case RANK_PARSER_NINJA: return "Parser Ninja";
        case RANK_ROUTER_ROMIN: return "Router Ronin";
        case RANK_PROTOCOL_KENSEI: return "Protocol Kensei";
        case RANK_ENCRYPTION_SHOGUN: return "Encryption Shogun";
        case RANK_MULTITHREAD_DAIMYO: return "Multithread Daimyo";
        case RANK_STREAM_SENSEI: return "Stream Sensei";
        case RANK_NETWORK_TENGU: return "Network Tengu";
        default: return "Unknown Rank";
    }
}

// Function to determine rank based on points (e.g., 10 points per course section)
Rank determine_rank(int points) {
    if (points >= 2000) return RANK_NETWORK_TENGU;
    if (points >= 1600) return RANK_STREAM_SENSEI;
    if (points >= 1300) return RANK_MULTITHREAD_DAIMYO;
    if (points >= 1000) return RANK_ENCRYPTION_SHOGUN;
    if (points >= 800) return RANK_PROTOCOL_KENSEI;
    if (points >= 600) return RANK_ROUTER_ROMIN;
    if (points >= 400) return RANK_PARSER_NINJA;
    if (points >= 267) return RANK_HTTP_BUSHI;
    if (points >= 68) return RANK_SOCKET_APPRENTICE;
    if (points >= 39) return RANK_ADDRESSINFO_DESHI;
    if (points >= 18) return RANK_LAYER_SHIKOMI;
    return RANK_PACKET_NOVICE;
}

// Example usage
int rank_def() {
    int completed_sections = 5; // Example: user completed 5 sections
    int points = completed_sections * 10; // 10 points per section
    Rank user_rank = determine_rank(points);
    
    printf("User has %d points. Rank: %s\n", points, get_rank_name(user_rank));
    
    return 0;
}
