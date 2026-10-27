#include <Tof.h>
#include <Arduino.h>

Tof::Tof()
    : lxIN(false),
      lxQ(false),
      llPT(0),
      llStartTime(0),
      Edge() {
}

void Tof::PT(unsigned long ilPT) {
    llPT = ilPT;
}

void Tof::IN(bool ixIN) {
    lxIN = ixIN;
}

void Tof::run() {
    Edge.run(lxIN);

    if (Edge.EdgeNeg()) {
        // fallende Flanke -> Startzeit merken
        llStartTime = millis();
    }

    if (!lxIN) {
        if ((millis() - llStartTime) >= llPT) {
            lxQ = false;
        } else {
            lxQ = true;
        }
    } else {
        lxQ = lxIN;
        llStartTime = millis();
    }
}

bool Tof::Q() const {
    return lxQ;
}

unsigned long Tof::ET() const {
    unsigned long tlElapsed = 0;
    if (lxIN) {
        tlElapsed = millis() - llStartTime;
    }
    return (tlElapsed > llPT) ? llPT : tlElapsed; // begrenzen
}
