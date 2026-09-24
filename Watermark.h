#ifndef WATERMARK_H
#define WATERMARK_H

#include <string>
#include <cstdint>

class Watermark {
public:
    struct Config {
        std::string text      = "跑路科技";
        int         rotation  = -30;
        int         fontSize  = 70;
        int         alpha     = 76;
        uint32_t    color     = 0xFFFF00FFu;
        int         width     = 1920;
        int         height    = 1080;
        int         spacingX  = 320;
        int         spacingY  = 180;
        bool        staggered = true;
        bool        enabled   = true;
        std::string bg        = "#f5f5f5";
        std::string font      = "'PingFang SC', 'Microsoft YaHei', "
                                 "'Noto Sans CJK SC', 'SimHei', sans-serif";
    };

    Watermark();
    explicit Watermark(const Config& cfg);

    void        setConfig(const Config& cfg);
    const Config& config() const;

    void setText(const std::string& t);
    void setRotation(int deg);
    void setSize(int px);
    void setAlpha(int a);
    void setColor(uint32_t argb);
    void setColorInt(int signedArgb);
    void setEnabled(bool on);
    void setCanvas(int w, int h);
    void setSpacing(int sx, int sy);

    bool saveSVG(const std::string& filename) const;
    std::string toSVG() const;

private:
    Config m_cfg;

    static std::string xmlEscape(const std::string& s);
    static std::string rgbHex(uint32_t argb);
    static double      toOpacity(int alpha);
    std::string buildTextLayer() const;
};

#endif