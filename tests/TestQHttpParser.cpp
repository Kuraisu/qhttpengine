/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QList>
#include <QObject>
#include <QTest>

#include "core/qhttpheader.h"
#include "util/qhttpparser.h"

typedef QList<QByteArray> QByteArrayList;
typedef QList<QHttpHeader> QHttpHeaderList;

Q_DECLARE_METATYPE(QHttpHeaderList)

class TestQHttpParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testSplit_data();
    void testSplit();

    void testParseRequest_data();
    void testParseRequest();
};

void TestQHttpParser::testSplit_data()
{
    QTest::addColumn<QByteArray>("original");
    QTest::addColumn<QByteArray>("delimiter");
    QTest::addColumn<int>("maxSplit");
    QTest::addColumn<QByteArrayList>("list");

    QTest::newRow("empty string")
            << QByteArray()
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "");

    QTest::newRow("no delimiter")
            << QByteArray("a")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a");

    QTest::newRow("single-char delimiter")
            << QByteArray("a,b,c")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("multi-char delimiter")
            << QByteArray("a::b::c")
            << QByteArray("::")
            << 0
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("empty parts")
            << QByteArray("a,,")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a" << "" << "");

    QTest::newRow("maxSplit")
            << QByteArray("a,a,a")
            << QByteArray(",")
            << 1
            << (QByteArrayList() << "a" << "a,a");
}

void TestQHttpParser::testSplit()
{
    QFETCH(QByteArray, original);
    QFETCH(QByteArray, delimiter);
    QFETCH(int, maxSplit);
    QFETCH(QByteArrayList, list);

    QCOMPARE(QHttpParser::split(original, delimiter, maxSplit), list);
}

void TestQHttpParser::testParseRequest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArray>("method");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<QHttpHeaderList>("headers");

    QTest::newRow("empty request")
            << QByteArray("")
            << false;

    QTest::newRow("bad request")
            << QByteArray("BAD REQUEST")
            << false;

    QTest::newRow("no headers")
            << QByteArray("GET /path HTTP/1.0")
            << true
            << QByteArray("GET")
            << QByteArray("/path")
            << QHttpHeaderList();

    QTest::newRow("multiple headers")
            << QByteArray("GET / HTTP/1.0\r\nX-Test1: test\r\nX-Test2: test")
            << true
            << QByteArray("GET")
            << QByteArray("/")
            << (QHttpHeaderList() << QHttpHeader("X-Test1", "test") << QHttpHeader("X-Test2", "test"));
}

void TestQHttpParser::testParseRequest()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, success);

    QByteArray outMethod;
    QByteArray outPath;
    QHttpHeaderList outHeaders;

    QCOMPARE(QHttpParser::parseRequest(data, outMethod, outPath, outHeaders), success);

    if(success) {
        QFETCH(QByteArray, method);
        QFETCH(QByteArray, path);
        QFETCH(QHttpHeaderList, headers);

        QCOMPARE(outMethod, method);
        QCOMPARE(outPath, path);
        QCOMPARE(outHeaders, headers);
    }
}

QTEST_MAIN(TestQHttpParser)
#include "TestQHttpParser.moc"