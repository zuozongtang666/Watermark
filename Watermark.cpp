#include "Watermark.h"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <algorithm>

Watermark::Watermark() = default;

Watermark::Watermark(const Config& cfg) : m_cfg(cfg) {}

void Watermark::setConfig(const Config& cfg) { m_cfg = cfg; }

const Watermark::Config& Watermark::config() const { return m_cfg; }

void Watermark::setText(const std::string& t)     { m_cfg.text = t; }
void Watermark::setRotation(int deg)               { m_cfg.rotation = deg; }
void Watermark::setSize(int px)                    { m_cfg.fontSize = (px < 1 ? 1 : px); }
void Watermark::setAlpha(int a)                    { m_cfg.alpha = std::clamp(a, 0, 255); }
void Watermark::setColor(uint32_t argb)            { m_cfg.color = argb; }
void Watermark::setColorInt(int signedArgb) {
    m_cfg.color = static_cast<uint32_t>(signedArgb);
}
void Watermark::setEnabled(bool on)                { m_cfg.enabled = on; }
void Watermark::setCanvas(int w, int h) {
    m_cfg.width  = (w < 1 ? 1 : w);
    m_cfg.height = (h < 1 ? 1 : h);
}
void Watermark::setSpacing(int sx, int sy) {
    m_cfg.spacingX = (sx < 1 ? 1 : sx);
    m_cfg.spacingY = (sy < 1 ? 1 : sy);
}

std::string Watermark::xmlEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '&':  out += "&amp;";  break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default:   out += c;
        }
    }
    return out;
}

std::string Watermark::rgbHex(uint32_t argb) {
    uint8_t r = (argb >> 16) & 0xFF;
    uint8_t g = (argb >> 8)  & 0xFF;
    uint8_t b =  argb        & 0xFF;
    std::ostringstream ss;
    ss << "#" << std::hex << std::setfill('0')
       << std::setw(2) << static_cast<int>(r)
       << std::setw(2) << static_cast<int>(g)
       << std::setw(2) << static_cast<int>(b);
    return ss.str();
}

double Watermark::toOpacity(int alpha) {
    if (alpha < 0)    alpha = 0;
    if (alpha > 255)  alpha = 255;
    return alpha / 255.0;
}

std::string Watermark::buildTextLayer() const {
    const int margin = static_cast<int>(
        std::hypot(static_cast<double>(m_cfg.width),
                   static_cast<double>(m_cfg.height)));
    const int startX = -margin;
    const int startY = -margin;
    const int endX   = m_cfg.width  + margin;
    const int endY   = m_cfg.height + margin;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);

    int rowIdx = 0;
    for (int y = startY; y <= endY; y += m_cfg.spacingY, ++rowIdx) {
        const bool  oddRow     = (rowIdx % 2) != 0;
        const int   rowOffset  = (m_cfg.staggered && oddRow) ? (m_cfg.spacingX / 2) : 0;
        for (int x = startX + rowOffset; x <= endX; x += m_cfg.spacingX) {
            ss << "<text "
               << "x=\"" << x << "\" "
               << "y=\"" << y << "\" "
               << "font-family=\"" << m_cfg.font << "\" "
               << "font-size=\"" << m_cfg.fontSize << "\" "
               << "fill=\"" << rgbHex(m_cfg.color) << "\" "
               << "fill-opacity=\"" << toOpacity(m_cfg.alpha) << "\">"
               << xmlEscape(m_cfg.text)
               << "</text>\n";
        }
    }
    return ss.str();
}

std::string Watermark::toSVG() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);

    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
       << "width=\"" << m_cfg.width << "\" "
       << "height=\"" << m_cfg.height << "\" "
       << "viewBox=\"0 0 " << m_cfg.width << " " << m_cfg.height << "\">\n";

    if (!m_cfg.bg.empty()) {
        ss << "<rect width=\"" << m_cfg.width << "\" height=\"" << m_cfg.height
           << "\" fill=\"" << m_cfg.bg << "\"/>\n";
    }

    if (m_cfg.enabled) {
        const double cx = m_cfg.width  / 2.0;
        const double cy = m_cfg.height / 2.0;
        ss << "<g transform=\"rotate(" << m_cfg.rotation
           << " " << cx << " " << cy << ")\">\n";
        ss << buildTextLayer();
        ss << "</g>\n";
    }

    ss << "</svg>\n";
    return ss.str();
}

bool Watermark::saveSVG(const std::string& filename) const {
    std::ofstream f(filename, std::ios::binary);
    if (!f) return false;
    f << toSVG();
    return static_cast<bool>(f);
}