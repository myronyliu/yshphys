#pragma once

class Game;
class GameObject;

class GameNode
{
	friend class Game;
public:
	GameObject* GetGameObject() const;
	GameNode* GetPrev() const;
	GameNode* GetNext() const;
private:
	GameNode();
	virtual ~GameNode();

	void BindGameObject(GameObject* gameObject);
	void Remove();
	void AppendTo(GameNode* prev);
	void PrependTo(GameNode* next);

	GameObject* m_gameObject;

	GameNode* m_next;
	GameNode* m_prev;

	Game* m_physicsScene;
};

class FreedGameNode
{
public:
	FreedGameNode();
	virtual ~FreedGameNode();

	GameNode* m_node;
	GameNode* m_precedingNode;
};
