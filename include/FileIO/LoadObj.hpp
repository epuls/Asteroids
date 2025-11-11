#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <unordered_map>
#include <cstdlib>

#include <Rendering/Vertex.h>


inline bool LoadOBJ(const std::string& path,
                    std::vector<Vertex>& outVertices,
                    std::vector<unsigned int>& outIndices)
{
    std::ifstream in(path);
    if (!in) return false;

    outVertices.clear();
    outIndices.clear();

    std::vector<std::array<float,3>> positions;
    std::vector<std::array<float,3>> normals;
    std::vector<std::array<float,3>> tex; // store 3, use first 2

    auto parseFaceTok = [](const std::string& s, int& vi, int& ti, int& ni){
        vi = ti = ni = 0;
        size_t a = s.find('/');
        if (a == std::string::npos) { vi = std::atoi(s.c_str()); return; }
        vi = std::atoi(s.substr(0, a).c_str());
        size_t b = s.find('/', a + 1);
        if (b == std::string::npos) { // v/vt
            std::string t = s.substr(a + 1);
            if (!t.empty()) ti = std::atoi(t.c_str());
            return;
        }
        std::string t = s.substr(a + 1, b - a - 1);
        std::string n = s.substr(b + 1);
        if (!t.empty()) ti = std::atoi(t.c_str());
        if (!n.empty()) ni = std::atoi(n.c_str());
    };

    std::string line;
    while (std::getline(in, line)) {
        // trim leading spaces
        size_t start = line.find_first_not_of(" \t\r");
        if (start == std::string::npos || line[start] == '#') continue;

        std::istringstream iss(line.substr(start));
        std::string tag; iss >> tag;

        if (tag == "v") {
            std::array<float,3> p{}; iss >> p[0] >> p[1] >> p[2];
            positions.push_back(p);
        } else if (tag == "vn") {
            std::array<float,3> n{}; iss >> n[0] >> n[1] >> n[2];
            normals.push_back(n);
        } else if (tag == "vt") {
            std::array<float,3> t{0.f,0.f,0.f};
            iss >> t[0];
            if (!(iss >> t[1])) t[1] = 0.f;
            if (!(iss >> t[2])) t[2] = 0.f;
            tex.push_back(t);
        } else if (tag == "f") {
            std::vector<unsigned int> faceIdx;
            faceIdx.reserve(8);

            std::string tok;
            while (iss >> tok) {
                int vi, ti, ni; parseFaceTok(tok, vi, ti, ni);

                // OBJ is 1-based; ignore negatives/zeros for simplicity
                int v = (vi > 0 && vi <= (int)positions.size()) ? vi - 1 : -1;
                int t = (ti > 0 && ti <= (int)tex.size())        ? ti - 1 : -1;
                int n = (ni > 0 && ni <= (int)normals.size())    ? ni - 1 : -1;

                Vertex vtx{};
                if (v >= 0) {
                    vtx.position[0]=positions[v][0];
                    vtx.position[1]=positions[v][1];
                    vtx.position[2]=positions[v][2];
                } else { vtx.position[0]=vtx.position[1]=vtx.position[2]=0.f; }

                if (t >= 0) {
                    vtx.uv[0]=tex[t][0];
                    vtx.uv[1]=tex[t][1];
                } else { vtx.uv[0]=vtx.uv[1]=0.f; }

                if (n >= 0) {
                    vtx.normal[0]=normals[n][0];
                    vtx.normal[1]=normals[n][1];
                    vtx.normal[2]=normals[n][2];
                } else { vtx.normal[0]=0.f; vtx.normal[1]=0.f; vtx.normal[2]=1.f; }

                unsigned int idx = (unsigned int)outVertices.size();
                outVertices.push_back(vtx);
                faceIdx.push_back(idx);
            }

            // Fan triangulation: (0, i, i+1)
            for (size_t i = 1; i + 1 < faceIdx.size(); ++i) {
                outIndices.push_back(faceIdx[0]);
                outIndices.push_back(faceIdx[i]);
                outIndices.push_back(faceIdx[i+1]);
            }
        }
        // ignore everything else
    }

    return true;
}
