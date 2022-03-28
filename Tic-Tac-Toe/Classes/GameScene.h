#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "cocos2d.h"
#include "AI2.h"
#include "UI.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    bool isLastMoveWon(int row, int column, int piece);

private:
    cocos2d::Rect _gridSpaces[3][3];
    cocos2d::Sprite* _gridPieces[3][3];
    cocos2d::Sprite* _gridSprite;

    int _gridArray[3][3];

    int _turn;
    int _gameState;


    void InitGridRects();
    void InitGridPieces();

    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    UI* ui;
};
#endif // __GAME_SCENE_H__