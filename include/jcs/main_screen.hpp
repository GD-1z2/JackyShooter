#include <jcs/screen.hpp>

class MainScreen : public Screen {
public:
    explicit MainScreen(JSGame &game);

    void update() override;

    void render() override;

    void onResize() override;

    void onFocus() override;

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;

private:
    GuiTextButton play_button;
    GuiTextInput ip_input;
    VertexBuffer title_vbo;
};
