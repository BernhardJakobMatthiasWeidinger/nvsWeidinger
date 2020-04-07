#include <iostream>
#include <chrono>

enum Verwendungshaeufigkeit {
    nie,
    selten,
    gelegentlich,
    regelmaessig,
    haeufig,
    staendig
};

enum SeilwareArt {
    Kletterseil,
    StatischesLastseil,
    Canyoningseil,
    Reepschnur,
    Bandmaterial
};

enum HartwareArt {
    Stahlschraubkarabiner,
    Tuber,
    Trage,
    Funkgeraet
};


struct Inventarobjekt {
    bool ausgeschieden{};
    std::string bemerkung{};
    std::chrono::time_point<std::chrono::system_clock> produktionsdatum;
};


struct Geraet : Inventarobjekt {
    std::string beschreibung{};
    int anzahl{};
};


struct Wartungsobjekt : Inventarobjekt {
    std::string name{};
    std::chrono::duration<double> wartungsintervall;
};


struct Wartung {
    std::chrono::time_point<std::chrono::system_clock> datum;
    std::string beschreibung{};
    std::string bemerkung{};
    bool erledigt{};
};


struct Seilware : Geraet {
    SeilwareArt typ;
    Verwendungshaeufigkeit verwendung;
};


struct Hartware : Geraet {
    HartwareArt typ;
};


struct Fahrzeug : Wartungsobjekt {
    std::string kennzeichen{};
};


struct Immobilie : Wartungsobjekt {
    std::string adresse{};
};
