#ifndef TEST_STYLEMANAGER_H
#define TEST_STYLEMANAGER_H

#include <QObject>
#include <QtTest>

class TestStyleManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Unit tests
    void testLoadValidFile();
    void testLoadNonexistentFile();
    void testSaveAndLoad();
    void testAvailableTemplates();
    
    // Property-based tests
    void testRoundTripProperty();
    void testRoundTripProperty_data();
};

#endif // TEST_STYLEMANAGER_H
