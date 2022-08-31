#include <jcs/game_screen.hpp>
#include <jcs/screen.hpp>

class InfoScreen : public Screen {
public:
    explicit InfoScreen(JSGame &game);

    ~InfoScreen() noexcept override;

    void update() override;

    void render() override;

    void onFocus() override;

    bool onKey(int key, int scancode, int action, int mods) override;

private:
    GameScreen &parent_screen;
};
