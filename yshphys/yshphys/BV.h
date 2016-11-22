#pragma once

// Bounding Volume Class
class BV
{
public:
	BV();
	virtual ~BV();

protected:

	BV* m_parent;
	BV* m_children[2];
};

