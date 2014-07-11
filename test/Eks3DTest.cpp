#include <QtTest>
#include "XLine.h"
#include "XPlane.h"
#include "XShape.h"

class Eks3DTest : public QObject
  {
  Q_OBJECT

public:
  Eks3DTest();
  virtual ~Eks3DTest();

private Q_SLOTS:
  void lineTest();
  void planeTest();
  void shapeTest();
  };

Eks3DTest::Eks3DTest()
  {
  }

Eks3DTest::~Eks3DTest()
  {
  }

void Eks3DTest::lineTest()
  {
  Eks::Line l1(Eks::Vector3D(0,0,0), Eks::Vector3D(1,0,0), Eks::Line::TwoPoints);
  Eks::Line l2(Eks::Vector3D(0,1,0), Eks::Vector3D(0,1,0), Eks::Line::PointAndDirection);

  float t1 = l1.closestPointOn(l2);
  float t2 = l2.closestPointOn(l1);
  QCOMPARE(t1, 0.0f);
  QCOMPARE(t2, -1.0f);

  QVERIFY(l1.sample(t1).isApprox(Eks::Vector3D::Zero()));
  QVERIFY(l2.sample(t2).isApprox(Eks::Vector3D::Zero()));

  l1.setPosition(Eks::Vector3D(0,0,1));

  t1 = l1.closestPointOn(l2);
  t2 = l2.closestPointOn(l1);
  QCOMPARE(t1, 0.0f);
  QCOMPARE(t2, -1.0f);

  QVERIFY(l1.sample(t1).isApprox(Eks::Vector3D(0,0,1)));
  QVERIFY(l2.sample(t2).isApprox(Eks::Vector3D::Zero()));

  l1.setPosition(Eks::Vector3D(1,0,0));

  t1 = l1.closestPointOn(l2);
  t2 = l2.closestPointOn(l1);
  QCOMPARE(t1, -1.0f);
  QCOMPARE(t2, -1.0f);

  QVERIFY(l1.sample(t1).isApprox(Eks::Vector3D::Zero()));
  QVERIFY(l2.sample(t2).isApprox(Eks::Vector3D::Zero()));

  QCOMPARE(l1.closestPointTo(Eks::Vector3D(0, 10, 0)), -1.0f);
  QCOMPARE(l2.closestPointTo(Eks::Vector3D(0, 10, 0)), 9.0f);
  }

void Eks3DTest::planeTest()
  {
  Eks::Plane p(Eks::Vector3D(10, 10, 10), Eks::Vector3D(10, 10, 10));
  Eks::Plane p2(Eks::Vector3D(10, 0, 0), Eks::Vector3D(10, 10, 0), Eks::Vector3D(10,10,10));

  QVERIFY(p.position().isApprox(Eks::Vector3D(10, 10, 10)));
  QVERIFY(p.normal().isApprox(Eks::Vector3D(10, 10, 10).normalized()));

  QCOMPARE(Eks::Plane(Eks::Vector3D(0,1,0), Eks::Vector3D(0,1,0)).distanceToPlane(Eks::Vector3D(0,1,0)), 0.0f);
  QCOMPARE(p.distanceToPlane(Eks::Vector3D(20,20,20)), Eks::Vector3D(10,10,10).norm());
  QCOMPARE((float)p.distanceToPlane(Eks::Vector3D(20,0,0)), -5.77350235f);

  p = Eks::Plane(Eks::Vector3D(10,0,10), Eks::Vector3D(1,0,1));

  Eks::Line l(Eks::Vector3D(10,0,10), Eks::Vector3D(0,0,20));
  QVERIFY(p.intersection(l) == std::numeric_limits<float>::infinity());

  l = Eks::Line(Eks::Vector3D(0,0,0), Eks::Vector3D(1,0,0));
  QCOMPARE(p.intersection(l), 20.0f);
  }

void Eks3DTest::shapeTest()
  {
  Eks::Shape s1 = Eks::Shape::create<Eks::Plane>(Eks::Vector3D(0,0,0), Eks::Vector3D(0,1,0));
  Eks::Shape s2 = Eks::Shape::create<Eks::Line>(Eks::Vector3D(0,0,0), Eks::Vector3D(1,0,0));

  QVERIFY(s1.is<Eks::Plane>());
  QVERIFY(!s1.is<Eks::Line>());
  QVERIFY(!s2.is<Eks::Plane>());
  QVERIFY(s2.is<Eks::Line>());

  Eks::Shape isct1 = s1.intersect(s2);
  QVERIFY(isct1.is<Eks::Vector3D>());

  s1.set<Eks::Line>(Eks::Vector3D(0,0,0), Eks::Vector3D(1,0,0));
  s2.set<Eks::Plane>(Eks::Vector3D(0,0,0), Eks::Vector3D(0,1,0));

  QVERIFY(!s1.is<Eks::Plane>());
  QVERIFY(s1.is<Eks::Line>());
  QVERIFY(s2.is<Eks::Plane>());
  QVERIFY(!s2.is<Eks::Line>());

  Eks::Shape isct2 = s1.intersect(s2);
  QVERIFY(isct2.is<Eks::Vector3D>());

  s1.set<Eks::Line>(Eks::Vector3D(0,0,0), Eks::Vector3D(1,0,0));
  s2.set<Eks::Line>(Eks::Vector3D(0,0,0), Eks::Vector3D(0,1,0));

  Eks::Shape isct3 = s1.intersect(s2);
  QVERIFY(isct3.is<Eks::Vector3D>());
  }

QTEST_APPLESS_MAIN(Eks3DTest)

#include "Eks3DTest.moc"
