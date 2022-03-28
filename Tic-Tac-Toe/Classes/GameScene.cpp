
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Definitions.h"


USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //ui = new UI(this, gameState);

    auto gameBackground = Sprite::create("res/Main Menu Background.png");
    gameBackground->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
    this->addChild(gameBackground);

    _gridSprite = Sprite::create("res/Grid.png");
    _gridSprite->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
    this->addChild(_gridSprite);

    InitGridRects();

    InitGridPieces();

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            _gridArray[i][j] = EMPTY_PIECE;
        }
    }

    //////////////////////////////////////////////////////////////////
    /*for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            CCLOG("AFTER INIT: Grid (%d, %d): %d", i, j, gridArray[i][j]);

        }
    }*/

    _gameState = STATE_PLAYING;

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(false);

    touchListener->onTouchBegan = [](Touch* touch, Event* event)
    {
        return true;
    };

    touchListener->onTouchMoved = [](Touch* touch, Event* event)
    {

    };

    touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

}

void GameScene::InitGridRects()
{
    float rowOrigin, colOrigin;
    for (int i = 0; i < 3; i++)
    {
        rowOrigin = _gridSprite->getBoundingBox().getMinY() + _gridSprite->getBoundingBox().size.height / 3 * (2 - i);
        for (int j = 0; j < 3; j++)
        {
            colOrigin = _gridSprite->getBoundingBox().getMinX() + _gridSprite->getBoundingBox().size.width / 3 * j;
            _gridSpaces[i][j] = Rect(colOrigin, rowOrigin, _gridSprite->getBoundingBox().size.width / 3, _gridSprite->getBoundingBox().size.height / 3);


            /*CCLOG("Init rect %.3f + %.3f, %.3f + %.3f",
                colOrigin,
                gridSprite->getBoundingBox().size.width / 3,
                rowOrigin,
                gridSprite->getBoundingBox().size.height / 3
            );*/
        }
    }
    CCLOG("InitGridRects!");
}

void GameScene::InitGridPieces()
{
    for (int i = 0; i < 3;i++)
    {
        for (int j = 0;j < 3;j++)
        {
            _gridPieces[i][j] = Sprite::create("res/X.png");
            _gridPieces[i][j]->setPosition(Vec2(_gridSprite->getPosition().x + _gridPieces[i][j]->getContentSize().width * (j - 1),
                _gridSprite->getPosition().y + _gridPieces[i][j]->getContentSize().height * (1 - i)));
            _gridPieces[i][j]->setVisible(false);
            //gridPieces[i][j]->setOpacity(0);
            this->addChild(_gridPieces[i][j]);
        }
    }
    CCLOG("InitGridPieces!");
}

void GameScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Rect playBoard = _gridSprite->getBoundingBox();
    auto touchPoint = touch->getLocation();

    if (_gameState = STATE_PLAYING)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                //CCLOG("Grid (%d, %d): %d", i, j, gridArray[i][j]);
                if (_gridSpaces[i][j].containsPoint(touchPoint))
                {
                    if (EMPTY_PIECE == _gridArray[i][j])
                    {
                        _gridArray[i][j] = PLAYER_PIECE;
                        if (PLAYER_PIECE == O_PIECE)
                            _gridPieces[i][j]->setTexture("res/O.png");
                        _gridPieces[i][j]->setVisible(true);
                        //gridPieces[i][j]->runAction(FadeIn::create(PIECE_FADE_IN_TIME));

                        if (isLastMoveWon(i, j, PLAYER_PIECE))
                        {
                            //Game Over
                            CCLOG("PLAYER WON");
                            GameScene::menuCloseCallback(this);
                        }
                        else if (!isMoveLeft(_gridArray))
                        {
                            //Game draw
                            CCLOG("GAME DRAW");
                            GameScene::menuCloseCallback(this);
                        }
                        else {
                            _gameState = STATE_AI_PLAYING;
                            Move aiMove = findBestMove(_gridArray);
                            _gridArray[aiMove.x][aiMove.y] = AI_PIECE;
                            if (AI_PIECE == O_PIECE)
                                _gridPieces[aiMove.x][aiMove.y]->setTexture("res/O.png");
                            _gridPieces[aiMove.x][aiMove.y]->setVisible(true);
                            //gridPieces[aiMove.x][aiMove.y]->runAction(FadeIn::create(PIECE_FADE_IN_TIME));

                            if (isLastMoveWon(aiMove.x, aiMove.y, AI_PIECE))
                            {
                                //Game Over
                                CCLOG("AI WON");
                                GameScene::menuCloseCallback(this);
                            }
                            else if (!isMoveLeft(_gridArray))
                            {
                                //Game draw
                                CCLOG("GAME DRAW");
                                GameScene::menuCloseCallback(this);
                            }

                            _gameState = STATE_PLAYING;
                        }
                    }
                }
            }
        }
    }
    //CCLOG("Touched at %.5f, %.5f",touchPoint.x,touchPoint.y);
}

bool GameScene::isLastMoveWon(int row, int column, int piece)
{
    //Game won diagonally: case 1
    if (row == column)
    {
        if (_gridArray[(row + 1) % 3][(column + 1) % 3] == piece && _gridArray[(row + 2) % 3][(column + 2) % 3] == piece)
        {
            if (piece == X_PIECE)
            {
                for (int i = 0; i < 3; i++)
                {
                    _gridPieces[i][i]->setTexture("res/X Win.png");
                }
            }
            else if (piece == O_PIECE)
            {
                for (int i = 0; i < 3; i++)
                {
                    _gridPieces[i][i]->setTexture("res/O Win.png");
                }
            }
            return true;
        }
    }

    //Game won diagonally: case 2
    if ((row + column) == 2)
    {
        if (_gridArray[(row + 1) % 3][(column - 1) % 3] == piece && _gridArray[(row + 2) % 3][(column - 2) % 3] == piece)
        {
            if (piece == X_PIECE)
            {
                for (int i = 0; i < 3; i++)
                {
                    _gridPieces[i][2 - i]->setTexture("res/X Win.png");
                }
            }
            else if (piece == O_PIECE)
            {
                for (int i = 0; i < 3; i++)
                {
                    _gridPieces[i][2 - i]->setTexture("res/O Win.png");
                }
            }
            return true;
        }
    }

    //Game won horizontally
    if (_gridArray[row][(column + 1) % 3] == piece && _gridArray[row][(column + 2) % 3] == piece)
    {
        if (piece == X_PIECE)
        {
            for (int col = 0; col < 3; col++)
            {
                _gridPieces[row][col]->setTexture("res/X Win.png");
            }
        }
        else if (piece == O_PIECE)
        {
            for (int col = 0; col < 3; col++)
            {
                _gridPieces[row][col]->setTexture("res/O Win.png");
            }
        }
        return true;
    }

    //Game won vertically
    if (_gridArray[(row + 1) % 3][column] == piece && _gridArray[(row + 2) % 3][column] == piece)
    {
        if (piece == X_PIECE)
        {
            for (int ro = 0; ro < 3; ro++)
            {
                _gridPieces[ro][column]->setTexture("res/X Win.png");
            }
        }
        else if (piece == O_PIECE)
        {
            for (int ro = 0; ro < 3; ro++)
            {
                _gridPieces[ro][column]->setTexture("res/O Win.png");
            }
        }
        return true;
    }

    return false;

}