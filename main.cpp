// SPDX-License-Identifier: MIT

#include "clang/AST/Stmt.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/ReplacementsYaml.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

namespace {

class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit MatchHandler(std::map<std::string, clang::tooling::Replacements> &replace) : replace_(replace) {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    const clang::Stmt *s{result.Nodes.getNodeAs<clang::Stmt>("ref")};
    if (nullptr == s) {
      return;
    }

    clang::tooling::Replacement r{*(result.SourceManager), s->getBeginLoc(), 0, "/*found*/"};
    std::ignore = replace_[r.getFilePath().str()].add(r);

    clang::DiagnosticsEngine &engine{result.Context->getDiagnostics()};
    const auto id = engine.getCustomDiagID(clang::DiagnosticsEngine::Warning, "%0");
    const auto source_range = clang::CharSourceRange::getTokenRange(s->getSourceRange());
    const auto fix_it = clang::FixItHint::CreateInsertion(s->getBeginLoc(), "/*found*/");
    engine.Report(s->getBeginLoc(), id) << "found" << source_range << fix_it;
  }

private:
  std::map<std::string, clang::tooling::Replacements> &replace_;
};

void Export(const std::map<std::string, clang::tooling::Replacements> &replacements) {
  for (const auto &r : replacements) {
    std::error_code ec;
    llvm::raw_fd_ostream file{r.first + ".yaml", ec, llvm::sys::fs::OF_None};
    if (ec) {
      llvm::errs() << llvm::format("Error opening replacement file: %s\n", ec.message().c_str());
      continue;
    }

    clang::tooling::TranslationUnitReplacements tu;
    tu.MainSourceFile = r.first;
    tu.Replacements.insert(tu.Replacements.end(), r.second.begin(), r.second.end());

    llvm::yaml::Output yaml{file};
    yaml << tu;
  }
}

} // namespace

int main(int argc, const char **argv) {
  llvm::cl::OptionCategory cat{"refactor"};
  clang::tooling::CommonOptionsParser op{argc, argv, cat};
  clang::tooling::RefactoringTool tool{op.getCompilations(), op.getSourcePathList()};

  MatchHandler handler{tool.getReplacements()};

  clang::ast_matchers::MatchFinder finder{};
  using namespace clang::ast_matchers;
  auto m = cxxOperatorCallExpr(hasArgument(0, hasType(cxxRecordDecl(hasName("Foo")))), isAssignmentOperator());
  finder.addMatcher(m.bind("ref"), &handler);

  if (int result = tool.run(clang::tooling::newFrontendActionFactory(&finder).get())) {
    return result;
  }

  Export(tool.getReplacements());

  return 0;
}
