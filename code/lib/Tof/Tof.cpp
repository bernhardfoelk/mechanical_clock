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
        lxRunning = true;
    }

    if (lxIN) {
        lxQ = true;
        lxRunning = false;
        llStartTime = millis();
    } else if (lxRunning && ((millis() - llStartTime) < llPT)) {
        lxQ = true; // Nachlaufzeit läuft noch
    } else {
        lxQ = false;
        lxRunning = false; // abgelaufen oder nie gestartet
    }

    /*
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
    */
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
