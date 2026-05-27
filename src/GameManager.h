// GameManager.h
class GameManager {
public:
    GameManager();
    void Update();
    void Draw() const;
    void HandleInput();

private:
    std::vector<std::unique_ptr<IGame>> games;
    int current = 0;
    float resetTimer = 0.0f;
    float resetDelay = 2.0f;
    GameState state;

    void nextGame();
};
