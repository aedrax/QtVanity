#include "test_findreplacebar.h"
#include "FindReplaceBar.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QSignalSpy>
#include <QRandomGenerator>

void TestFindReplaceBar::initTestCase()
{
    // Setup code if needed
}

void TestFindReplaceBar::cleanupTestCase()
{
    // Cleanup code if needed
}

// ============================================================================
// Unit Tests
// ============================================================================

void TestFindReplaceBar::testInitialState()
{
    QTextEdit editor;
    FindReplaceBar bar(&editor);
    
    // Initially hidden
    QVERIFY(!bar.isVisible());
    
    // No matches initially
    QCOMPARE(bar.matchCount(), 0);
    QCOMPARE(bar.currentMatchIndex(), -1);
    
    // Empty search text
    QVERIFY(bar.searchText().isEmpty());
    
    // Case sensitivity off by default
    QVERIFY(!bar.isCaseSensitive());
}

void TestFindReplaceBar::testShowFindMode()
{
    QTextEdit editor;
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    
    QVERIFY(bar.isVisible());
}

void TestFindReplaceBar::testShowReplaceMode()
{
    QTextEdit editor;
    FindReplaceBar bar(&editor);
    
    bar.showReplaceMode();
    
    QVERIFY(bar.isVisible());
}

void TestFindReplaceBar::testHide()
{
    QTextEdit editor;
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    QVERIFY(bar.isVisible());
    
    bar.hide();
    QVERIFY(!bar.isVisible());
}

void TestFindReplaceBar::testSetSearchText()
{
    QTextEdit editor;
    editor.setPlainText("Hello World Hello");
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setSearchText("Hello");
    
    QCOMPARE(bar.searchText(), QString("Hello"));
    QCOMPARE(bar.matchCount(), 2);
}

void TestFindReplaceBar::testCaseSensitivity()
{
    QTextEdit editor;
    editor.setPlainText("Hello hello HELLO");
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    
    // Case insensitive (default)
    bar.setCaseSensitive(false);
    bar.setSearchText("hello");
    QCOMPARE(bar.matchCount(), 3);
    
    // Case sensitive
    bar.setCaseSensitive(true);
    bar.setSearchText("hello");
    QCOMPARE(bar.matchCount(), 1);
}

void TestFindReplaceBar::testEmptySearch()
{
    QTextEdit editor;
    editor.setPlainText("Some content here");
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setSearchText("");
    
    QCOMPARE(bar.matchCount(), 0);
    QCOMPARE(bar.currentMatchIndex(), -1);
}

void TestFindReplaceBar::testNoMatches()
{
    QTextEdit editor;
    editor.setPlainText("Hello World");
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setSearchText("xyz");
    
    QCOMPARE(bar.matchCount(), 0);
    QCOMPARE(bar.currentMatchIndex(), -1);
}

// ============================================================================
// Property-Based Tests
// ============================================================================

/**
 * Feature: find-replace, Property 1: Match Count Consistency
 * 
 * For any search query and document content, the reported match count
 * SHALL equal the actual number of highlighted regions in the document.
 */
void TestFindReplaceBar::testMatchCountConsistency_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<bool>("caseSensitive");
    QTest::addColumn<int>("expectedCount");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool for generating random content
    QStringList words = {
        "color", "background", "border", "padding", "margin",
        "QPushButton", "QLabel", "QLineEdit", "red", "blue",
        "green", "white", "black", "solid", "none", "px"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Generate random document content (3-10 words)
        QString content;
        int numWords = 3 + (rng->generate() % 8);
        for (int w = 0; w < numWords; ++w) {
            if (w > 0) content += " ";
            content += words[rng->generate() % words.size()];
        }
        
        // Pick a random search query from the words
        QString query = words[rng->generate() % words.size()];
        
        // Random case sensitivity
        bool caseSens = (rng->generate() % 2) == 0;
        
        // Calculate expected count manually (non-overlapping matches, like Qt's find)
        int expectedCount = 0;
        QString searchContent = caseSens ? content : content.toLower();
        QString searchQuery = caseSens ? query : query.toLower();
        int pos = 0;
        while ((pos = searchContent.indexOf(searchQuery, pos)) != -1) {
            ++expectedCount;
            pos += searchQuery.length(); // Move past the match (non-overlapping)
        }
        
        QTest::newRow(qPrintable(QString("match_count_%1").arg(i)))
            << content << query << caseSens << expectedCount;
    }
    
    // Edge cases
    QTest::newRow("empty_document") << QString("") << QString("test") << false << 0;
    QTest::newRow("empty_query") << QString("Hello World") << QString("") << false << 0;
    QTest::newRow("single_match") << QString("Hello World") << QString("Hello") << false << 1;
    QTest::newRow("multiple_matches") << QString("test test test") << QString("test") << false << 3;
    QTest::newRow("overlapping_pattern") << QString("aaa") << QString("aa") << false << 1; // Qt find is non-overlapping
    QTest::newRow("case_sensitive_match") << QString("Hello hello HELLO") << QString("Hello") << true << 1;
    QTest::newRow("case_insensitive_match") << QString("Hello hello HELLO") << QString("hello") << false << 3;
    QTest::newRow("no_match") << QString("Hello World") << QString("xyz") << false << 0;
    QTest::newRow("special_chars") << QString("a.b.c") << QString(".") << false << 2;
    QTest::newRow("whitespace_query") << QString("a b c") << QString(" ") << false << 2;
    QTest::newRow("newline_content") << QString("line1\nline2\nline1") << QString("line1") << false << 2;
    QTest::newRow("qss_content") << QString("QPushButton { color: red; } QPushButton:hover { color: blue; }") 
                                  << QString("QPushButton") << false << 2;
}

void TestFindReplaceBar::testMatchCountConsistency()
{
    // Feature: find-replace, Property 1: Match Count Consistency
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(bool, caseSensitive);
    QFETCH(int, expectedCount);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setCaseSensitive(caseSensitive);
    bar.setSearchText(searchQuery);
    
    // Property: reported match count equals expected count
    QCOMPARE(bar.matchCount(), expectedCount);
    
    // Property: number of highlighted regions equals match count
    QList<QTextEdit::ExtraSelection> selections = editor.extraSelections();
    QCOMPARE(selections.count(), expectedCount);
    
    // Property: if matches exist, current index is valid
    if (expectedCount > 0) {
        QVERIFY(bar.currentMatchIndex() >= 0);
        QVERIFY(bar.currentMatchIndex() < expectedCount);
    } else {
        QCOMPARE(bar.currentMatchIndex(), -1);
    }
}


/**
 * Feature: find-replace, Property 2: Navigation Wrap-Around
 * 
 * For any non-empty set of matches, navigating forward from the last match
 * SHALL position the cursor at the first match, and navigating backward
 * from the first match SHALL position the cursor at the last match.
 */
void TestFindReplaceBar::testNavigationWrapAround_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<int>("expectedMatchCount");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool for generating random content with guaranteed matches
    QStringList words = {
        "color", "background", "border", "padding", "margin",
        "QPushButton", "QLabel", "QLineEdit", "red", "blue"
    };
    
    // Generate 100 random test cases with guaranteed matches
    for (int i = 0; i < 100; ++i) {
        // Pick a random word to search for
        QString searchWord = words[rng->generate() % words.size()];
        
        // Generate document content that includes the search word multiple times
        int numOccurrences = 2 + (rng->generate() % 5); // 2-6 occurrences
        QString content;
        
        for (int j = 0; j < numOccurrences; ++j) {
            if (j > 0) content += " ";
            content += searchWord;
            // Add some random words between occurrences
            int numFillers = 1 + (rng->generate() % 3);
            for (int k = 0; k < numFillers; ++k) {
                QString filler;
                do {
                    filler = words[rng->generate() % words.size()];
                } while (filler == searchWord); // Avoid adding more matches
                content += " " + filler;
            }
        }
        
        QTest::newRow(qPrintable(QString("wrap_around_%1").arg(i)))
            << content << searchWord << numOccurrences;
    }
    
    // Edge cases
    QTest::newRow("single_match") << QString("Hello World") << QString("Hello") << 1;
    QTest::newRow("two_matches") << QString("test foo test") << QString("test") << 2;
    QTest::newRow("three_matches") << QString("a b a c a") << QString("a") << 3;
    QTest::newRow("many_matches") << QString("x x x x x x x x x x") << QString("x") << 10;
    QTest::newRow("qss_selectors") << QString("QPushButton { } QLabel { } QPushButton:hover { }") 
                                   << QString("QPushButton") << 2;
}

void TestFindReplaceBar::testNavigationWrapAround()
{
    // Feature: find-replace, Property 2: Navigation Wrap-Around
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(int, expectedMatchCount);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setSearchText(searchQuery);
    
    // Verify we have the expected matches
    QCOMPARE(bar.matchCount(), expectedMatchCount);
    
    if (expectedMatchCount == 0) {
        return; // Skip navigation tests for no matches
    }
    
    // Property: After search, current index should be 0 (first match)
    QCOMPARE(bar.currentMatchIndex(), 0);
    
    // Property: Navigate forward through all matches
    for (int i = 1; i < expectedMatchCount; ++i) {
        bar.findNext();
        QCOMPARE(bar.currentMatchIndex(), i);
    }
    
    // Property: At last match, findNext() should wrap to first match
    QCOMPARE(bar.currentMatchIndex(), expectedMatchCount - 1); // At last match
    bar.findNext();
    QCOMPARE(bar.currentMatchIndex(), 0); // Should wrap to first
    
    // Property: At first match, findPrevious() should wrap to last match
    QCOMPARE(bar.currentMatchIndex(), 0); // At first match
    bar.findPrevious();
    QCOMPARE(bar.currentMatchIndex(), expectedMatchCount - 1); // Should wrap to last
    
    // Property: Navigate backward through all matches
    for (int i = expectedMatchCount - 2; i >= 0; --i) {
        bar.findPrevious();
        QCOMPARE(bar.currentMatchIndex(), i);
    }
    
    // Property: Verify we're back at first match
    QCOMPARE(bar.currentMatchIndex(), 0);
    
    // Property: Multiple wrap-arounds should work consistently
    // Go forward twice around the entire list
    for (int round = 0; round < 2; ++round) {
        for (int i = 0; i < expectedMatchCount; ++i) {
            int expectedIndex = (bar.currentMatchIndex() + 1) % expectedMatchCount;
            bar.findNext();
            QCOMPARE(bar.currentMatchIndex(), expectedIndex);
        }
    }
}


/**
 * Feature: find-replace, Property 4: Case Sensitivity Correctness
 * 
 * For any search query, when case-sensitive mode is enabled, all matches
 * SHALL have identical character casing to the query; when disabled,
 * matches MAY differ in casing.
 */
void TestFindReplaceBar::testCaseSensitivityCorrectness_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<bool>("caseSensitive");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool with various case variations
    QStringList baseWords = {
        "color", "background", "border", "padding", "margin",
        "button", "label", "widget", "style", "theme"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Pick a random base word
        QString baseWord = baseWords[rng->generate() % baseWords.size()];
        
        // Create case variations of the word
        QStringList variations;
        variations << baseWord;                                    // lowercase
        variations << baseWord.toUpper();                          // UPPERCASE
        variations << (baseWord[0].toUpper() + baseWord.mid(1));   // Capitalized
        
        // Build document with multiple case variations
        QString content;
        int numWords = 3 + (rng->generate() % 6); // 3-8 words
        for (int w = 0; w < numWords; ++w) {
            if (w > 0) content += " ";
            // Mix in case variations and other words
            if (rng->generate() % 2 == 0) {
                content += variations[rng->generate() % variations.size()];
            } else {
                // Add a different word to create non-matches
                QString otherWord;
                do {
                    otherWord = baseWords[rng->generate() % baseWords.size()];
                } while (otherWord == baseWord);
                content += otherWord;
            }
        }
        
        // Pick a random variation as the search query
        QString query = variations[rng->generate() % variations.size()];
        
        // Random case sensitivity setting
        bool caseSens = (rng->generate() % 2) == 0;
        
        QTest::newRow(qPrintable(QString("case_sens_%1").arg(i)))
            << content << query << caseSens;
    }
    
    // Edge cases
    QTest::newRow("exact_match_sensitive") << QString("Hello World") << QString("Hello") << true;
    QTest::newRow("exact_match_insensitive") << QString("Hello World") << QString("Hello") << false;
    QTest::newRow("lowercase_query_sensitive") << QString("Hello HELLO hello") << QString("hello") << true;
    QTest::newRow("lowercase_query_insensitive") << QString("Hello HELLO hello") << QString("hello") << false;
    QTest::newRow("uppercase_query_sensitive") << QString("Hello HELLO hello") << QString("HELLO") << true;
    QTest::newRow("uppercase_query_insensitive") << QString("Hello HELLO hello") << QString("HELLO") << false;
    QTest::newRow("mixed_case_query_sensitive") << QString("Hello HELLO hello") << QString("Hello") << true;
    QTest::newRow("mixed_case_query_insensitive") << QString("Hello HELLO hello") << QString("Hello") << false;
    QTest::newRow("no_match_sensitive") << QString("hello hello hello") << QString("HELLO") << true;
    QTest::newRow("all_match_insensitive") << QString("hello HELLO Hello") << QString("HeLLo") << false;
    QTest::newRow("qss_property_sensitive") << QString("background-color: red; Background-Color: blue;") 
                                            << QString("background-color") << true;
    QTest::newRow("qss_property_insensitive") << QString("background-color: red; Background-Color: blue;") 
                                              << QString("background-color") << false;
}

void TestFindReplaceBar::testCaseSensitivityCorrectness()
{
    // Feature: find-replace, Property 4: Case Sensitivity Correctness
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(bool, caseSensitive);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    FindReplaceBar bar(&editor);
    
    bar.showFindMode();
    bar.setCaseSensitive(caseSensitive);
    bar.setSearchText(searchQuery);
    
    int matchCount = bar.matchCount();
    
    if (matchCount == 0) {
        // No matches - verify this is correct
        if (caseSensitive) {
            // Case-sensitive: query should not exist with exact casing
            QVERIFY(!documentContent.contains(searchQuery));
        }
        // Case-insensitive with no matches means query doesn't exist at all
        return;
    }
    
    // Get the extra selections (highlighted matches)
    QList<QTextEdit::ExtraSelection> selections = editor.extraSelections();
    QCOMPARE(selections.count(), matchCount);
    
    // Property: Verify each match has correct casing behavior
    for (const QTextEdit::ExtraSelection &selection : selections) {
        QString matchedText = selection.cursor.selectedText();
        
        if (caseSensitive) {
            // Property: When case-sensitive, all matches SHALL have identical casing to query
            QCOMPARE(matchedText, searchQuery);
        } else {
            // Property: When case-insensitive, matches MAY differ in casing
            // but must match when compared case-insensitively
            QCOMPARE(matchedText.toLower(), searchQuery.toLower());
        }
    }
    
    // Additional property: Toggling case sensitivity should update results immediately
    int originalCount = matchCount;
    bar.setCaseSensitive(!caseSensitive);
    
    // After toggle, match count may change
    int newCount = bar.matchCount();
    
    // Toggle back and verify we get the original count
    bar.setCaseSensitive(caseSensitive);
    QCOMPARE(bar.matchCount(), originalCount);
}


/**
 * Feature: find-replace, Property 3: Replace All Completeness
 * 
 * For any search query with N matches, executing "Replace All" SHALL
 * result in exactly N replacements and zero remaining matches for the
 * original search query.
 */
void TestFindReplaceBar::testReplaceAllCompleteness_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<QString>("replacementText");
    QTest::addColumn<int>("expectedReplacements");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool for generating random content
    QStringList words = {
        "color", "background", "border", "padding", "margin",
        "QPushButton", "QLabel", "QLineEdit", "red", "blue",
        "green", "white", "black", "solid", "none", "px"
    };
    
    // Replacement word pool
    QStringList replacements = {
        "REPLACED", "new_value", "changed", "updated", "modified"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Pick a random word to search for
        QString searchWord = words[rng->generate() % words.size()];
        
        // Generate document content that includes the search word multiple times
        int numOccurrences = 1 + (rng->generate() % 5); // 1-5 occurrences
        QString content;
        
        for (int j = 0; j < numOccurrences; ++j) {
            if (j > 0) content += " ";
            content += searchWord;
            // Add some random words between occurrences
            int numFillers = 1 + (rng->generate() % 3);
            for (int k = 0; k < numFillers; ++k) {
                QString filler;
                do {
                    filler = words[rng->generate() % words.size()];
                } while (filler == searchWord); // Avoid adding more matches
                content += " " + filler;
            }
        }
        
        // Pick a random replacement text
        QString replacement = replacements[rng->generate() % replacements.size()];
        
        QTest::newRow(qPrintable(QString("replace_all_%1").arg(i)))
            << content << searchWord << replacement << numOccurrences;
    }
    
    // Edge cases
    QTest::newRow("single_match") << QString("Hello World") << QString("Hello") << QString("Hi") << 1;
    QTest::newRow("multiple_matches") << QString("test foo test bar test") << QString("test") << QString("check") << 3;
    QTest::newRow("empty_replacement") << QString("remove this remove") << QString("remove") << QString("") << 2;
    QTest::newRow("longer_replacement") << QString("a b a") << QString("a") << QString("longer_text") << 2;
    QTest::newRow("shorter_replacement") << QString("longword x longword") << QString("longword") << QString("s") << 2;
    QTest::newRow("same_replacement") << QString("same same same") << QString("same") << QString("same") << 3;
    QTest::newRow("qss_property") << QString("color: red; color: blue;") << QString("color") << QString("background") << 2;
}

void TestFindReplaceBar::testReplaceAllCompleteness()
{
    // Feature: find-replace, Property 3: Replace All Completeness
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(QString, replacementText);
    QFETCH(int, expectedReplacements);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    FindReplaceBar bar(&editor);
    
    bar.showReplaceMode();
    bar.setSearchText(searchQuery);
    
    // Verify initial match count
    QCOMPARE(bar.matchCount(), expectedReplacements);
    
    // Set replacement text (access via child widget)
    QLineEdit *replaceInput = bar.findChild<QLineEdit*>();
    // Find the second QLineEdit (replace input)
    QList<QLineEdit*> lineEdits = bar.findChildren<QLineEdit*>();
    QVERIFY(lineEdits.size() >= 2);
    lineEdits[1]->setText(replacementText);
    
    // Perform Replace All
    bar.replaceAll();
    
    // Property: After Replace All, zero remaining matches for original query
    bar.setSearchText(searchQuery);
    
    // If replacement text is different from search query, there should be no matches
    if (replacementText != searchQuery) {
        QCOMPARE(bar.matchCount(), 0);
    }
    
    // Property: The replacement text should now appear in the document
    QString newContent = editor.toPlainText();
    if (!replacementText.isEmpty() && replacementText != searchQuery) {
        // Count occurrences of replacement text
        int replacementCount = 0;
        int pos = 0;
        while ((pos = newContent.indexOf(replacementText, pos)) != -1) {
            ++replacementCount;
            pos += replacementText.length();
        }
        QCOMPARE(replacementCount, expectedReplacements);
    }
}


/**
 * Feature: find-replace, Property 5: Replace Preserves Non-Matches
 * 
 * For any document and search query, performing "Replace All" SHALL not
 * modify any text that was not part of a match.
 */
void TestFindReplaceBar::testReplacePreservesNonMatches_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<QString>("replacementText");
    QTest::addColumn<QStringList>("nonMatchSegments");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool for generating random content
    QStringList words = {
        "color", "background", "border", "padding", "margin",
        "QPushButton", "QLabel", "QLineEdit", "red", "blue"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Pick a random word to search for
        QString searchWord = words[rng->generate() % words.size()];
        
        // Generate non-match segments
        QStringList nonMatches;
        QString content;
        
        int numSegments = 2 + (rng->generate() % 4); // 2-5 segments
        for (int j = 0; j < numSegments; ++j) {
            // Add a non-match segment
            QString segment;
            int segmentLen = 1 + (rng->generate() % 3);
            for (int k = 0; k < segmentLen; ++k) {
                QString word;
                do {
                    word = words[rng->generate() % words.size()];
                } while (word == searchWord);
                if (k > 0) segment += " ";
                segment += word;
            }
            nonMatches << segment;
            
            if (!content.isEmpty()) content += " ";
            content += segment;
            
            // Add the search word between segments (except after last)
            if (j < numSegments - 1) {
                content += " " + searchWord;
            }
        }
        
        QString replacement = "REPLACED";
        
        QTest::newRow(qPrintable(QString("preserve_%1").arg(i)))
            << content << searchWord << replacement << nonMatches;
    }
    
    // Edge cases
    QTest::newRow("simple_preserve") 
        << QString("keep target keep") << QString("target") << QString("replaced") 
        << QStringList({"keep", "keep"});
    QTest::newRow("multiple_segments") 
        << QString("aaa target bbb target ccc") << QString("target") << QString("X") 
        << QStringList({"aaa", "bbb", "ccc"});
    QTest::newRow("qss_preserve") 
        << QString("QPushButton { color: red; }") << QString("color") << QString("background") 
        << QStringList({"QPushButton {", ": red; }"});
}

void TestFindReplaceBar::testReplacePreservesNonMatches()
{
    // Feature: find-replace, Property 5: Replace Preserves Non-Matches
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(QString, replacementText);
    QFETCH(QStringList, nonMatchSegments);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    FindReplaceBar bar(&editor);
    
    bar.showReplaceMode();
    bar.setSearchText(searchQuery);
    
    // Set replacement text
    QList<QLineEdit*> lineEdits = bar.findChildren<QLineEdit*>();
    QVERIFY(lineEdits.size() >= 2);
    lineEdits[1]->setText(replacementText);
    
    // Perform Replace All
    bar.replaceAll();
    
    // Property: All non-match segments should still be present in the document
    QString newContent = editor.toPlainText();
    for (const QString &segment : nonMatchSegments) {
        // Each non-match segment should still exist in the document
        QVERIFY2(newContent.contains(segment), 
                 qPrintable(QString("Non-match segment '%1' was modified or removed").arg(segment)));
    }
}


/**
 * Feature: find-replace, Property 6: Undo Atomicity for Replace All
 * 
 * For any "Replace All" operation that performs N replacements, a single
 * undo operation SHALL restore all N original matches.
 */
void TestFindReplaceBar::testUndoAtomicity_data()
{
    QTest::addColumn<QString>("documentContent");
    QTest::addColumn<QString>("searchQuery");
    QTest::addColumn<QString>("replacementText");
    QTest::addColumn<int>("expectedReplacements");
    
    QRandomGenerator *rng = QRandomGenerator::global();
    
    // Word pool for generating random content
    QStringList words = {
        "color", "background", "border", "padding", "margin",
        "QPushButton", "QLabel", "QLineEdit", "red", "blue"
    };
    
    // Generate 100 random test cases
    for (int i = 0; i < 100; ++i) {
        // Pick a random word to search for
        QString searchWord = words[rng->generate() % words.size()];
        
        // Generate document content that includes the search word multiple times
        int numOccurrences = 2 + (rng->generate() % 4); // 2-5 occurrences
        QString content;
        
        for (int j = 0; j < numOccurrences; ++j) {
            if (j > 0) content += " ";
            content += searchWord;
            // Add some random words between occurrences
            int numFillers = 1 + (rng->generate() % 3);
            for (int k = 0; k < numFillers; ++k) {
                QString filler;
                do {
                    filler = words[rng->generate() % words.size()];
                } while (filler == searchWord);
                content += " " + filler;
            }
        }
        
        QString replacement = "REPLACED";
        
        QTest::newRow(qPrintable(QString("undo_%1").arg(i)))
            << content << searchWord << replacement << numOccurrences;
    }
    
    // Edge cases
    QTest::newRow("two_replacements") << QString("test foo test") << QString("test") << QString("check") << 2;
    QTest::newRow("three_replacements") << QString("a b a c a") << QString("a") << QString("X") << 3;
    QTest::newRow("five_replacements") << QString("x 1 x 2 x 3 x 4 x") << QString("x") << QString("Y") << 5;
    QTest::newRow("qss_replacements") << QString("color: red; color: blue; color: green;") 
                                      << QString("color") << QString("background") << 3;
}

void TestFindReplaceBar::testUndoAtomicity()
{
    // Feature: find-replace, Property 6: Undo Atomicity for Replace All
    
    QFETCH(QString, documentContent);
    QFETCH(QString, searchQuery);
    QFETCH(QString, replacementText);
    QFETCH(int, expectedReplacements);
    
    QTextEdit editor;
    editor.setPlainText(documentContent);
    
    // Store original content
    QString originalContent = editor.toPlainText();
    
    FindReplaceBar bar(&editor);
    bar.showReplaceMode();
    bar.setSearchText(searchQuery);
    
    // Verify initial match count
    QCOMPARE(bar.matchCount(), expectedReplacements);
    
    // Set replacement text
    QList<QLineEdit*> lineEdits = bar.findChildren<QLineEdit*>();
    QVERIFY(lineEdits.size() >= 2);
    lineEdits[1]->setText(replacementText);
    
    // Perform Replace All
    bar.replaceAll();
    
    // Verify content changed
    QString afterReplaceContent = editor.toPlainText();
    QVERIFY(afterReplaceContent != originalContent);
    
    // Property: A single undo operation SHALL restore all N original matches
    editor.undo();
    
    // Verify content is restored to original
    QString afterUndoContent = editor.toPlainText();
    QCOMPARE(afterUndoContent, originalContent);
    
    // Verify all original matches are restored
    // Clear and re-set search text to force a re-search after undo
    bar.setSearchText("");
    bar.setSearchText(searchQuery);
    QCOMPARE(bar.matchCount(), expectedReplacements);
}
