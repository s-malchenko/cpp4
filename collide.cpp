#include "geo2d.h"
#include "game_object.h"

#include "test_runner.h"

#include <vector>
#include <memory>

using namespace std;

// Определите классы Unit, Building, Tower и Fence так, чтобы они наследовались от
// GameObject и реализовывали его интерфейс.

class Unit : public GameObject
{
public:
    explicit Unit(geo2d::Point geometry) : _geometry(geometry)
    {}

    geo2d::Point Get() const;
    bool Collide(const GameObject &that) const override;
    bool CollideWith(const Unit &that) const;
    bool CollideWith(const Building &that) const;
    bool CollideWith(const Tower &that) const;
    bool CollideWith(const Fence &that) const;

private:
    geo2d::Point _geometry;
};

class Building : public GameObject
{
public:
    explicit Building(geo2d::Rectangle geometry) : _geometry(geometry)
    {}

    geo2d::Rectangle Get() const;
    bool Collide(const GameObject &that) const override;
    bool CollideWith(const Unit &that) const override;
    bool CollideWith(const Building &that) const override;
    bool CollideWith(const Tower &that) const override;
    bool CollideWith(const Fence &that) const override;

private:
    geo2d::Rectangle _geometry;
};

class Tower : public GameObject
{
public:
    explicit Tower(geo2d::Circle geometry) : _geometry(geometry)
    {}

    geo2d::Circle Get() const;
    bool Collide(const GameObject &that) const override;
    bool CollideWith(const Unit &that) const override;
    bool CollideWith(const Building &that) const override;
    bool CollideWith(const Tower &that) const override;
    bool CollideWith(const Fence &that) const override;

private:
    geo2d::Circle _geometry;
};

class Fence : public GameObject
{
public:
    explicit Fence(geo2d::Segment geometry) : _geometry(geometry)
    {}

    geo2d::Segment Get() const;
    bool Collide(const GameObject &that) const override;
    bool CollideWith(const Unit &that) const override;
    bool CollideWith(const Building &that) const override;
    bool CollideWith(const Tower &that) const override;
    bool CollideWith(const Fence &that) const override;

private:
    geo2d::Segment _geometry;
};

#define METHOD_GET(Class, geometryType) \
geo2d::geometryType Class::Get() const  \
{                                       \
    return _geometry;                   \
}

#define METHOD_COLLIDE(Class)                       \
bool Class::Collide(const GameObject &that) const   \
{                                                   \
    return that.CollideWith(*this);                 \
}
#define METHOD_COLLIDE_WITH(MainClass, OtherClass)          \
bool MainClass::CollideWith(const OtherClass &that) const   \
{                                                           \
    return geo2d::Collide(_geometry, that.Get());           \
}

METHOD_GET(Unit, Point)
METHOD_COLLIDE(Unit)
METHOD_COLLIDE_WITH(Unit, Unit)
METHOD_COLLIDE_WITH(Unit, Building)
METHOD_COLLIDE_WITH(Unit, Tower)
METHOD_COLLIDE_WITH(Unit, Fence)

METHOD_GET(Building, Rectangle)
METHOD_COLLIDE(Building)
METHOD_COLLIDE_WITH(Building, Unit)
METHOD_COLLIDE_WITH(Building, Building)
METHOD_COLLIDE_WITH(Building, Tower)
METHOD_COLLIDE_WITH(Building, Fence)

METHOD_GET(Tower, Circle)
METHOD_COLLIDE(Tower)
METHOD_COLLIDE_WITH(Tower, Unit)
METHOD_COLLIDE_WITH(Tower, Building)
METHOD_COLLIDE_WITH(Tower, Tower)
METHOD_COLLIDE_WITH(Tower, Fence)

METHOD_GET(Fence, Segment)
METHOD_COLLIDE(Fence)
METHOD_COLLIDE_WITH(Fence, Unit)
METHOD_COLLIDE_WITH(Fence, Building)
METHOD_COLLIDE_WITH(Fence, Tower)
METHOD_COLLIDE_WITH(Fence, Fence)

bool Collide(const GameObject &first, const GameObject &second)
{
    return first.Collide(second);
}

void TestAddingNewObjectOnMap()
{
    // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то объекты,
    // и мы хотим добавить на неё новый, например, построить новое здание или башню.
    // Мы можем его добавить, только если он не пересекается ни с одним из существующих.
    using namespace geo2d;

    const vector<shared_ptr<GameObject>> game_map =
    {
        make_shared<Unit>(Point{3, 3}),
        make_shared<Unit>(Point{5, 5}),
        make_shared<Unit>(Point{3, 7}),
        make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
        make_shared<Tower>(Circle{Point{9, 4}, 1}),
        make_shared<Tower>(Circle{Point{10, 7}, 1}),
        make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
    };

    for (size_t i = 0; i < game_map.size(); ++i)
    {
        Assert(
            Collide(*game_map[i], *game_map[i]),
            "An object doesn't collide with itself: " + to_string(i)
        );

        for (size_t j = 0; j < i; ++j)
        {
            Assert(
                !Collide(*game_map[i], *game_map[j]),
                "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
            );
        }
    }

    auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
    ASSERT(!Collide(*new_warehouse, *game_map[0]));
    ASSERT( Collide(*new_warehouse, *game_map[1]));
    ASSERT(!Collide(*new_warehouse, *game_map[2]));
    ASSERT( Collide(*new_warehouse, *game_map[3]));
    ASSERT( Collide(*new_warehouse, *game_map[4]));
    ASSERT(!Collide(*new_warehouse, *game_map[5]));
    ASSERT(!Collide(*new_warehouse, *game_map[6]));

    auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
    ASSERT(!Collide(*new_defense_tower, *game_map[0]));
    ASSERT(!Collide(*new_defense_tower, *game_map[1]));
    ASSERT(!Collide(*new_defense_tower, *game_map[2]));
    ASSERT( Collide(*new_defense_tower, *game_map[3]));
    ASSERT( Collide(*new_defense_tower, *game_map[4]));
    ASSERT(!Collide(*new_defense_tower, *game_map[5]));
    ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestAddingNewObjectOnMap);
    return 0;
}
