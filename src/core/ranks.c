#include "../models/models.h"
#include <stdio.h>

const char* get_rank_name(Rank rank) {
    switch (rank) {
        case RANK_PACKET_NOVICE: return "Packet Novice";
        case RANK_LAYER_SHIKOMI: return "Layer Shikomi";
        case RANK_ADDRESSINFO_DESHI: return "Addressinfo Deshi";
        case RANK_SOCKET_APPRENTICE: return "Socket Apprentice";
        case RANK_HTTP_BUSHI: return "HTTP Bushi";
        case RANK_PARSER_NINJA: return "Parser Ninja";
        case RANK_ROUTER_RONIN: return "Router Ronin";
        case RANK_PROTOCOL_KENSEI: return "Protocol Kensei";
        case RANK_ENCRYPTION_SHOGUN: return "Encryption Shogun";
        case RANK_MULTITHREAD_DAIMYO: return "Multithread Daimyo";
        case RANK_STREAM_SENSEI: return "Stream Sensei";
        case RANK_NETWORK_TENGU: return "Network Tengu";
        default: return "Unknown Rank";
    }
}

Rank determine_rank(int points) {
    if (points >= 2000) return RANK_NETWORK_TENGU;
    if (points >= 1600) return RANK_STREAM_SENSEI;
    if (points >= 1300) return RANK_MULTITHREAD_DAIMYO;
    if (points >= 1000) return RANK_ENCRYPTION_SHOGUN;
    if (points >= 800) return RANK_PROTOCOL_KENSEI;
    if (points >= 600) return RANK_ROUTER_RONIN;
    if (points >= 400) return RANK_PARSER_NINJA;
    if (points >= 269) return RANK_HTTP_BUSHI;
    if (points >= 68) return RANK_SOCKET_APPRENTICE;
    if (points >= 39) return RANK_ADDRESSINFO_DESHI;
    if (points >= 18) return RANK_LAYER_SHIKOMI;
    return RANK_PACKET_NOVICE;
}

Rank get_next_rank(Rank r) {
    switch (r) {
        case RANK_PACKET_NOVICE: return RANK_LAYER_SHIKOMI;
        case RANK_LAYER_SHIKOMI: return RANK_ADDRESSINFO_DESHI;
        case RANK_ADDRESSINFO_DESHI: return RANK_SOCKET_APPRENTICE;
        case RANK_SOCKET_APPRENTICE: return RANK_HTTP_BUSHI;
        case RANK_HTTP_BUSHI: return RANK_PARSER_NINJA;
        case RANK_PARSER_NINJA: return RANK_ROUTER_RONIN;
        case RANK_ROUTER_RONIN: return RANK_PROTOCOL_KENSEI;
        case RANK_PROTOCOL_KENSEI: return RANK_ENCRYPTION_SHOGUN;
        case RANK_ENCRYPTION_SHOGUN: return RANK_MULTITHREAD_DAIMYO;
        case RANK_MULTITHREAD_DAIMYO: return RANK_STREAM_SENSEI;
        case RANK_STREAM_SENSEI: return RANK_NETWORK_TENGU;
        default: return r;
    }
}

// Example usage
int rank_def() {
    int completed_sections = 5; // Example: user completed 5 sections
    int points = completed_sections * 10; // 10 points per section
    Rank user_rank = determine_rank(points);
    
    printf("User has %d points. Rank: %s\n", points, get_rank_name(user_rank));
    
    return 0;
}
