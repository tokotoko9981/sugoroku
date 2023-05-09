# include <Siv3D.hpp>

struct GameData {
	int32 player = 2;
};

using App = SceneManager<String, GameData>;

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init)
		: IScene{ init } {

	}

	void update() override {

		m_startTransition.update(m_startButton.mouseOver());
		m_exitTransition.update(m_exitButton.mouseOver());

		if (m_startButton.mouseOver() || m_exitButton.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (m_startButton.leftClicked()) {
			// ゲームシーンへ
			changeScene(U"Game", 1.5s);
		}
		
		else if (m_exitButton.leftClicked()) {
			// 終了
			System::Exit();
		}

		if (SimpleGUI::Button(U"+", Vec2{ 330, 450 }, unspecified, (pluscheck == true))) {
			getData().player++;
		}

		if (SimpleGUI::Button(U"-", Vec2{ 430, 450 }, unspecified, (minuscheck == true))) {
			getData().player--;
		}

		

		if (getData().player == 2) minuscheck = false;
		else minuscheck = true;

		if (getData().player == 9) pluscheck = false;
		else pluscheck = true;
	}

	void draw() const override {

		Scene::SetBackground(Palette::White);

		FontAsset(U"TitleFont")(U"形容詞すごろく").drawAt(400, 100, Palette::Black);

		

		m_startButton.draw(ColorF{ 0.5, m_startTransition.value() }).drawFrame(2);
		m_exitButton.draw(ColorF{ 0.5, m_exitTransition.value() }).drawFrame(2);

		FontAsset(U"Menu")(U"PLAY").drawAt(m_startButton.center(), ColorF{ 0.25 });
		FontAsset(U"Menu")(U"EXIT").drawAt(m_exitButton.center(), ColorF{ 0.25 });

		FontAsset(U"PlayerFont")(U"プレイ人数: {}"_fmt(getData().player)).drawAt( Scene::Center().moveBy(0, 100) , ColorF{ 0.5 });
	}

private:

	Rect m_startButton{ Arg::center = Scene::Center(), 200, 60};
	Transition m_startTransition{ 0.4s, 0.2s };
	Rect m_exitButton{ Arg::center = Scene::Center().movedBy(0, 250), 200, 60 };
	Transition m_exitTransition{ 0.4s, 0.2s };
	bool pluscheck = true;
	bool minuscheck = false;

};

// ゲームシーン
class Game : public App::Scene {

public:

	Game(const InitData& init)
		: IScene{ init } {

		for (int32 i = 0; i < getData().player; i++) {
			piece << Circle{ 30 + 30 * (i % 3), 30 + 30 * (i /3), 15};
		}

		for (int32 i = 0; i < getData().player; i++) {
			footpiece << Circle{ 30 + 30 * (i % 3), 30 + 30 * (i / 3), 15 };
		}
	}

	void update() override {

		turn = count % getData().player;

		if (KeyQ.down()) {
			changeScene(U"Title", 1.5s);
		}

		if (SimpleGUI::Button(U"Roll", Vec2{ 25, 550 }, unspecified, (turncheck == true))) {
			resultnum = Random(result.size() - 2);
			turncheck = false;
		}

		if (KeySpace.down()) {
			count++;
			turncheck = true;
			resultnum = result.size() - 1;
		}

		if (KeyLeft.pressed() && not leftLine.intersects(piece[turn]) && not inRightLine.intersects(piece[turn])) {
			piece[turn].moveBy( Scene::DeltaTime() * -200, 0 );
		}

		if (KeyRight.pressed() && not rightLine.intersects(piece[turn]) && not inLeftLine.intersects(piece[turn])) {
			piece[turn].moveBy(Scene::DeltaTime() * 200, 0);
		}

		if (KeyUp.pressed() && not upperLine.intersects(piece[turn]) && not inUnderLine.intersects(piece[turn])) {
			piece[turn].moveBy( 0, Scene::DeltaTime() * -200);
		}

		if (KeyDown.pressed() && not underLine.intersects(piece[turn]) && not inUpperLine.intersects(piece[turn])) {
			piece[turn].moveBy(0, Scene::DeltaTime() * 200);
		}
	}

	void draw() const override {
		Scene::SetBackground(Palette::White);

		
		goal.draw(ColorF{ 0.25 });
		FontAsset(U"PlayerFont")(U"GOAL").draw(Arg::bottomRight = goalpos, goalcheck.contains(piece[turn]) ? Palette::White : Palette::Black );


		for (int32 i = 0; i < getData().player; i++) {
			footpiece[i].draw(HSV{ 360.0 / getData().player * i , 0.25 });
		}

		for (int32 i = 0; i < getData().player; i++) {
			piece[i].draw(HSV{ 360.0 / getData().player * i });
			FontAsset(U"Piece")(i + 1).drawAt(piece[i].center, ColorF{ 0.1 });
		}

		upperLine.draw(8, ColorF{ 0.5 });
		underLine.draw(6, ColorF{ 0.5 });
		leftLine.draw(8, ColorF{ 0.5 });
		rightLine.draw(8, ColorF{ 0.5 });

		inUpperLine.draw(LineStyle::RoundCap, 8, ColorF{ 0.5 });
		inUnderLine.draw(LineStyle::RoundCap, 8, ColorF{ 0.5 });
		inLeftLine.draw(LineStyle::RoundCap, 8, ColorF{ 0.5 });
		inRightLine.draw(LineStyle::RoundCap, 8, ColorF{ 0.5 });
		
		FontAsset(U"PlayerFont")(U"プレイヤー{}のターン"_fmt(turn + 1)).draw(Arg::bottomRight = playerpos, HSV{ 360.0 / getData().player * turn });
		FontAsset(U"Result")(U"{}"_fmt(result[resultnum])).drawAt(Scene::Center().moveBy(0, -50), Palette::Black);
	}

private:

	Array<Circle> piece;
	Array<Circle> footpiece;

	const Array<String> result = {
		U"すこし", U"めっちゃ", U"それとなく", U"たくさん", U"引くほど", U"そこそこ" ,U"まあまあ", U"いいかんじに", U"ちょっとだけ", U"かなり", U"ある程度",
		U"おもいっきり", U"なし崩してきに", U"痛いくらい", U"しっかりと", U"驚くほど", U"見えないくらい", U"面白いくらい", U"思ったより", U"大きく",
		U"小さく", U"多く", U"少なく", U"虚無", U"塵のように", U"信じられないほど", U"とても", U"ほんの気持ちだけ", U"小さめに", U"大きめに", U"少なめに", U"多めに",
		U"一瞬", U"怖いくらい", U"一回休み", U"すこし戻る", U"まあまあ戻る", U"かなり戻る", U""
	};

	int32 resultnum = result.size() - 1;
	int32 count = 0;
	int32 turn = 0;

	bool turncheck = true;

	const Vec2 playerpos{ 800, 600 };
	const Vec2 goalpos{ 795, 495 };

	const Rect goal{ 720, 450, 80, 50 };
	const Rect goalcheck{ 720, 450, 120, 75 };


	const Line upperLine{ 0, 0, 800, 0 };
	const Line underLine{ 0, 502, 800, 502 };
	const Line leftLine{ 0, 0, 0, 500 };
	const Line rightLine{ 800, 0, 800, 500 };

	const Line inUpperLine{ 161, 101, 639, 101 };
	const Line inUnderLine{ 161, 399, 639, 399 };
	const Line inLeftLine{ 161, 101, 161, 399 };
	const Line inRightLine{ 639, 101, 639, 399 };
};

void Main() {

	Window::SetTitle(U"形容詞すごろく");
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"PlayerFont", 25, Typeface::Bold);
	FontAsset::Register(U"Menu", FontMethod::MSDF, 40, Typeface::Medium);
	FontAsset::Register(U"Ranking", 40, Typeface::Heavy);
	FontAsset::Register(U"GameScore", 30, Typeface::Light);
	FontAsset::Register(U"Piece", 20, Typeface::Regular);
	FontAsset::Register(U"Result", 60, Typeface::Bold);

	App manager;
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");

	// フェードイン・フェードアウト時の画面の色
	manager.setFadeColor(Palette::Black);

	while (System::Update()) {

		if (not manager.update()) {
			break;
		}
	}
}
