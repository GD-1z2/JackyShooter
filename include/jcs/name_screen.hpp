#include <jcs/screen.hpp>

class NameScreen : public Screen {
public:
    explicit NameScreen(JSGame &game);

    void update() override;

    void render() override;

    bool onKey(int key, int scancode, int action, int mods) override;

private:
    GuiTextButton join_button, cancel_button;
    GuiTextInput name_input;
};
