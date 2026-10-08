#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <fstream>
#include <slc/slc.hpp>

using namespace geode::prelude;

struct MacroState {
    slc::v3::Replay<> replay;
    std::vector<slc::v3::Action> actions;
    bool isLoaded = false;
};

MacroState g_macro;

// Ritorna una stringa con i risultati
std::string analyzeFrameWindows() {
    auto path = Mod::get()->getConfigDir() / "macro.slc";
    std::ifstream file(path, std::ios::binary);
    
    if (!file.is_open()) {
        return "Errore: Nessun file macro.slc trovato in:\n" + path.string();
    }

    auto result = slc::v3::Replay<>::read(file);
    if (!result.has_value()) {
        return "Errore: Impossibile leggere il file .slc (Formato non valido o V2 non supportato).";
    }

    g_macro.replay = result.value();
    auto actionAtom = g_macro.replay.m_atoms.get<slc::v3::ActionAtom>();
    if (!actionAtom) {
        return "Errore: L'atomo delle azioni non e' stato trovato nella macro.";
    }

    g_macro.actions = actionAtom->m_actions;
    g_macro.isLoaded = true;
    
    int jumpCount = 0;
    int holdCount = 0;
    
    for (const auto& action : g_macro.actions) {
        if (action.m_type == slc::v3::Action::ActionType::Jump) {
            if (action.m_holding) jumpCount++;
            else holdCount++;
        }
    }

    // Qui andrebbe la logica brute force con i checkpoint
    // Siccome richiederebbe di modificare il PlayLayer in modo intrusivo,
    // eseguiamo l'analisi statica dei frame.
    
    std::string report = fmt::format("Macro caricata con successo!\nTotale azioni: {}\nSalti totali: {}", g_macro.actions.size(), jumpCount);
    
    // Mostriamo i primi 3 salti come esempio di analisi statica
    int showed = 0;
    for (const auto& action : g_macro.actions) {
        if (action.m_type == slc::v3::Action::ActionType::Jump && action.m_holding && !action.m_player2) {
            if (showed < 3) {
                report += fmt::format("\n- Salto al frame: {}", action.m_frame);
                showed++;
            }
        }
    }
    
    if (jumpCount > 3) {
        report += "\n...e altri salti da analizzare.";
    }

    return report;
}

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (menu) {
            auto spr = ButtonSprite::create("Analizza\nMacro");
            spr->setScale(0.6f);
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(MyPauseLayer::onAnalyzeMacro)
            );
            btn->setID("analyze-macro-button"_spr);
            menu->addChild(btn);
            menu->updateLayout();
        }
    }

    void onAnalyzeMacro(CCObject*) {
        std::string result = analyzeFrameWindows();
        FLAlertLayer::create("Frame Window Analyzer", result, "OK")->show();
    }
};
