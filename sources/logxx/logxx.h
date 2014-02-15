#ifndef LOG_HELPER_H
#define	LOG_HELPER_H

#include <mutex>
#include <string>
#include <ostream>
#include <map>
#include <memory>

namespace logxx {

	enum LogLevel {
		debug = 0,
		notice = 1,
		warning = 2,
		info = 3,
		error = 4,
		nothing = 5
	};

        class Log {
        public:
                Log(const std::string& label = std::string());
                Log(const Log&, const std::string& label, bool inheritLevel = true);
                virtual ~Log();

                void OverrideLogLevel();
                void OverrideLogLevel(LogLevel);
                static void GlobalLogLevel(LogLevel);
                static LogLevel GlobalLogLevel();

                void OverrideStream();
                void OverrideStream(const std::shared_ptr<std::ostream> &);
                void OverrideStream(const std::string & fileName);
                static void SetStream(std::ostream &);

                template<class ...Args>
                std::ostream& operator()(LogLevel, Args... args);

                static std::ostream& endl(std::ostream&);
        private:
                static std::mutex lock;
                static const std::map<LogLevel, std::string> levelMessages;

                static std::ostream cnull, *defaultStream;
                std::shared_ptr<std::ostream> overrideStream;

                std::string label;

                static LogLevel globalLogLevel;
                std::shared_ptr<LogLevel> overrideLogLevel;

                void AddLabel(const std::string &newLabel);
                LogLevel GetLogLevel() const;
                std::ostream& GetStream();
                template<class T, class ...Args>
                std::ostream& Print(std::ostream&, T t, Args... args) const;
                std::ostream& Print(std::ostream&) const;
        };
        
        std::ostream& endl(std::ostream&);
        void GlobalLogLevel(LogLevel);
        LogLevel GlobalLogLevel();
        void SetStream(std::ostream &);
        
} //namespace logxx

#define S_LOG(label) \
        static logxx::Log log(cLog, label)

#define D_LOG(label) \
        logxx::Log log(cLog, label)

#include "logxx.tcc"

#endif	/* LOG_HELPER_H */

