#include "Watermark.h"
#include <iostream>

int main() {
    Watermark wm;

    wm.setText("跑路科技");
    wm.setRotation(-30);
    wm.setSize(70);
    wm.setAlpha(76);
    wm.setColorInt(-65281);
    wm.setEnabled(true);

    const std::string out = "watermark.svg";
    if (!wm.saveSVG(out)) {
        std::cerr << "Failed to write " << out << std::endl;
        return 1;
    }

    const std::string svg = wm.toSVG();
    std::cout << "Watermark written: " << out
              << " (" << svg.size() << " bytes, "
              << (wm.config().enabled ? "on" : "off") << ")"
              << std::endl;

    wm.setEnabled(false);
    wm.saveSVG("watermark_off.svg");
    std::cout << "Disabled sample written: watermark_off.svg" << std::endl;
    return 0;
}