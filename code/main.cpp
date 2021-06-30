#include "TTUtility.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <list>

/*
fixlog 8
msg recv 35=8|9 49 56 34
XTAI.log 日期

ls -R |grep log/2020-12-21
./O9268F6/ROCO/log/2020-12-21:
./O9268F7/ROCO/log/2020-12-21:
./O9268F8/ROCO/log/2020-12-21:
./O9268F9/ROCO/log/2020-12-21:
./T9268F2/XTAI/log/2020-12-21:
./T9268F3/XTAI/log/2020-12-21:
./T9268F4/XTAI/log/2020-12-21:
./T9268F5/XTAI/log/2020-12-21:

cp -R /home/stella/tradetron/O9268F6/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/O9268F7/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/O9268F8/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/O9268F9/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/T9268F2/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/T9268F3/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/T9268F4/ /home/jimmy/kgi/logCollection/data/
cp -R /home/stella/tradetron/T9268F5/ /home/jimmy/kgi/logCollection/data/

grep -m 1 -n '\[FIXLOG\]' ./T9268F2/XTAI/log/2020-12-21/XTAI.log
38:[FIXLOG]10:26:06.752053  send 8=FIX.4.49=20635=D49=T9268F256=XTAI34=0001452=20201221-02:26:04.00057=050=845010001=010000=310002=010004=N60=20201221-02:26:04.00038=00140=259=044=0010.401=139377937=a000055=2331  54=111=00001745800010=096

grep -m 1 -n '\[FIXLOG\]' ./T9268F2/XTAI/log/2020-12-21/XTAI.log | cut -d ':' -f 1

tail --lines=+38 ./T9268F2/XTAI/log/2020-12-21/XTAI.log
tail -n +38 ./T9268F2/XTAI/log/2020-12-21/XTAI.log

*/

int main(int argc, char *argv[]) {
    const std::string cfg_fname = argc < 2 ? "paths.cfg" : argv[1];
    std::ifstream configFile(cfg_fname, std::ifstream::in);
    std::list<std::tuple<const std::string, std::unique_ptr<std::ifstream>, const std::string, std::unique_ptr<std::fstream>>> files;
    std::string targetFilename;
    if (!!configFile) {
        std::string keyword;
        for (std::string key; std::getline(configFile, key, '='); ) {
            if (key == "target") {
                if (std::string tgtPath_tmp; std::getline(configFile, tgtPath_tmp)) {
                    static const std::string paraToday {"%TODAY%"};
                    static const std::string today = getTime(std::chrono::system_clock::now(), "%Y%m%d", false);
                    targetFilename = tgtPath_tmp.replace(tgtPath_tmp.find(paraToday), paraToday.size(), today);
                }
            } else if (std::string srcPath_tmp; std::getline(configFile, srcPath_tmp)) {
                static const std::string paraToday {"%TODAY%"};
                static const std::string today = getTime(std::chrono::system_clock::now(), "%Y-%m-%d", false);
                const std::string stepPath {"./"+today+"_"+key+".step"};
                const std::string srcPath {srcPath_tmp.replace(srcPath_tmp.find(paraToday), paraToday.size(), today)};
                files.emplace_back(std::make_tuple(srcPath, std::make_unique<std::ifstream>(srcPath, std::ifstream::in), stepPath, std::make_unique<std::fstream>(stepPath, std::fstream::in | std::fstream::out)));
            }
        }
    }
    std::ofstream targetFile(targetFilename, std::ofstream::out | std::ofstream::app);
    if (!targetFile.is_open()) {
        switch (targetFile.rdstate()) {
            case std::ios_base::goodbit: std::cout << "failed (goodbit) to open targetFile: " << targetFilename << std::endl; break;  // no error
            case std::ios_base::badbit:  std::cout << "failed (badbit) to open targetFile: "  << targetFilename << std::endl; break;  // irrecoverable stream error
            case std::ios_base::failbit: std::cout << "failed (failbit) to open targetFile: " << targetFilename << std::endl; break;  // input/output operation failed (formatting or extraction error)
            case std::ios_base::eofbit:  std::cout << "failed (eofbit) to open targetFile: "  << targetFilename << std::endl; break;  // associated input sequence has reached end-of-file
            default: std::cout << "failed(" << targetFile.rdstate() << ") to open targetFile: " << targetFilename << std::endl; break;
        }
        return -1;
    } else {
        std::cout << "target file: " << targetFilename << std::endl;
    }
    for (auto &[srcPath,srcFile_ptr,stepPath,stepFile_ptr]: files) {
        auto &srcFile = *srcFile_ptr;
        auto &stepFile = *stepFile_ptr;
        if (!srcFile.is_open()) {
            switch (srcFile.rdstate()) {
                case std::ios_base::goodbit: std::cout << "failed (goodbit) to open srcPath: " << srcPath << std::endl; break;      // no error
                case std::ios_base::badbit:  std::cout << "failed (badbit) to open srcPath: "  << srcPath << std::endl; return -1;  // irrecoverable stream error
                case std::ios_base::failbit: std::cout << "failed (failbit) to open srcPath: " << srcPath << std::endl; break;      // input/output operation failed (formatting or extraction error)
                case std::ios_base::eofbit:  std::cout << "failed (eofbit) to open srcPath: "  << srcPath << std::endl; break;      // associated input sequence has reached end-of-file
                default: std::cout << "failed(" << srcFile.rdstate() << ") to open srcPath: " << srcPath << std::endl; return -1;
            }
        } else {
            std::cout << "source file: " << srcPath << std::endl;
        }
        if (!stepFile.is_open()) {
            stepFile.clear();
            stepFile.open(stepPath, std::ios::out);
            stepFile.close();
            stepFile.open(stepPath, std::fstream::in | std::fstream::out);
            if (!stepFile.is_open()) {
                switch (stepFile.rdstate()) {
                    case std::ios_base::goodbit: std::cout << "failed (goodbit) to open stepFile: " << stepPath << std::endl; break;  // no error
                    case std::ios_base::badbit:  std::cout << "failed (badbit) to open stepFile: "  << stepPath << std::endl; break;  // irrecoverable stream error
                    case std::ios_base::failbit: std::cout << "failed (failbit) to open stepFile: " << stepPath << std::endl; break;  // input/output operation failed (formatting or extraction error)
                    case std::ios_base::eofbit:  std::cout << "failed (eofbit) to open stepFile: "  << stepPath << std::endl; break;  // associated input sequence has reached end-of-file
                    default: std::cout << "failed(" << stepFile.rdstate() << ") to open stepFile: " << stepPath << std::endl; break;
                }
                return -1;
            }
        } else {
            long pos;
            stepFile >> pos;
            if (!!stepFile) {
                std::cout << "step(" << pos << ") file: " << stepPath << std::endl;
                srcFile.seekg(pos);
            } else {
                switch (stepFile.rdstate()) {
                    case std::ios_base::goodbit: std::cout << "failed (goodbit) to read stepFile: " << stepPath << std::endl; break;  // no error
                    case std::ios_base::badbit:  std::cout << "failed (badbit) to read stepFile: "  << stepPath << std::endl; break;  // irrecoverable stream error
                    case std::ios_base::failbit: std::cout << "failed (failbit) to read stepFile: " << stepPath << std::endl; break;  // input/output operation failed (formatting or extraction error)
                    case std::ios_base::eofbit:  std::cout << "failed (eofbit) to read stepFile: "  << stepPath << std::endl; break;  // associated input sequence has reached end-of-file
                    default: std::cout << "failed(" << stepFile.rdstate() << ") to read stepFile: " << stepPath << std::endl; break;
                }
                stepFile.clear();
            }
        }
    }
    for (;;std::this_thread::sleep_for(std::chrono::milliseconds(100))) {
        for (auto &[srcPath,srcFile_ptr,stepPath,stepFile_ptr]: files) {
            auto &srcFile = *srcFile_ptr;
            auto &stepFile = *stepFile_ptr;
            if (!!srcFile) {
                switch (srcFile.rdstate()) {
                    case std::ios_base::goodbit: /*std::cout << "failed(goodbit) from srcFile:" << srcPath << std::endl;*/ break;                   // no error
                    case std::ios_base::badbit:  std::cout << "failed(badbit) from srcFile:"  << srcPath << std::endl; return -1;                   // irrecoverable stream error
                    case std::ios_base::failbit: std::cout << "failed(failbit) from srcFile:" << srcPath << std::endl; srcFile.clear(); break;      // input/output operation failed (formatting or extraction error)
                    case std::ios_base::eofbit:  std::cout << "failed(eofbit) from srcFile:"  << srcPath << std::endl; srcFile.clear(); break;      // associated input sequence has reached end-of-file
                    default: std::cout << "failed(" << srcFile.rdstate() << ") from srcFile:" << srcPath << std::endl; return -1;
                }
            }
            if (srcFile.eof()) {
                srcFile.clear();
            }
            if (!!stepFile) switch (stepFile.rdstate()) {
                case std::ios_base::goodbit: /*std::cout << "failed(goodbit) from stepFile:" << srcPath << std::endl;*/ break;                   // no error
                case std::ios_base::badbit:  std::cout << "failed(badbit) from stepFile:"  << srcPath << std::endl; return -1;                   // irrecoverable stream error
                case std::ios_base::failbit: std::cout << "failed(failbit) from stepFile:" << srcPath << std::endl; stepFile.clear(); break;      // input/output operation failed (formatting or extraction error)
                case std::ios_base::eofbit:  std::cout << "failed(eofbit) from stepFile:"  << srcPath << std::endl; stepFile.clear(); break;      // associated input sequence has reached end-of-file
                default: std::cout << "failed(" << stepFile.rdstate() << ") from stepFile:" << srcPath << std::endl; return -1;
            }
            bool goOn = false;
            std::string buf;
            for (std::string line; std::getline(srcFile, line); std::this_thread::yield()) {
                line = buf + line;
                const long pos = srcFile.tellg();
                lineParser:
                if (std::string::size_type thefront = 0;
                    (goOn = buf.empty() && goOn)
                    || ((thefront = line.find("[FIXLOG]" )) != std::string::npos && (thefront = line.find(" send 8=", thefront+8)) != std::string::npos)
                    || ((thefront = line.find("[MSG]"    )) != std::string::npos && (thefront = line.find(" recv 8=", thefront+5)) != std::string::npos && (line.find("35=8", thefront+8) != std::string::npos || line.find("35=9", thefront+8) != std::string::npos))
                ) {
                    buf.clear();
                    if (std::string::size_type thetail = line.find("10=", ++thefront); thetail != std::string::npos && (thetail = line.find("", ++thetail)) != std::string::npos) {
                        targetFile << line.substr(goOn? thefront-1: thefront, ++thetail) << std::endl;
                        goOn = false;
                        if (line.size() > thetail) {
                            line = line.substr(thetail, line.size());
                            goto lineParser;
                        }
                    } else {
                        srcFile.clear();
                        std::cout<<"===> goOn: "<<goOn<<" broken: "<<line<<std::endl;
                        buf += line;
                        goOn = true;
                        std::this_thread::sleep_for(std::chrono::milliseconds(6000));
                        continue;
                    }
                } else if (std::string::size_type thetail = line.find("10="); thetail != std::string::npos && (thetail = line.find("", ++thetail)) != std::string::npos && line.size() > ++thetail) {
                    std::cout<<"!got end position: "<<line<<std::endl;
                    line = line.substr(thetail, line.size());
                    goto lineParser;
                }
                buf.clear();
                stepFile.seekp(0);
                stepFile << pos << std::endl;
            }
        }
    }
    return 0;
}
