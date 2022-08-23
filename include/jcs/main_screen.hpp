#include <jcs/screen.hpp>

class MainScreen : public Screen {
public:
    explicit MainScreen(JSGame &game);

    ~MainScreen() override;

    void update() override;

    void render() override;

    void onFocus() override;

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;

private:
    GuiTextButton *play_button;
    GuiTextInput *ip_input, *name_input;
};
