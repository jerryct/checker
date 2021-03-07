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
  void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
    const clang::Stmt *s{result.Nodes.getNodeAs<clang::Stmt>("ref")};
    if (nullptr == s) {
      return;
    }

    if (replacement_.MainSourceFile.empty()) {
      const clang::FileID main_file{result.SourceManager->getMainFileID()};
      const clang::FileEntry *file_entry{result.SourceManager->getFileEntryForID(main_file)};
      if (nullptr == file_entry) {
        return;
      }
      replacement_.MainSourceFile = file_entry->getName().str();
    }

    replacement_.Replacements.emplace_back(*(result.SourceManager), s->getBeginLoc(), 0, "/*found*/");

    clang::DiagnosticsEngine &engine{result.Context->getDiagnostics()};
    const auto id = engine.getCustomDiagID(clang::DiagnosticsEngine::Warning, "%0");
    const auto source_range = clang::CharSourceRange::getTokenRange(s->getSourceRange());
    const auto fix_it = clang::FixItHint::CreateInsertion(s->getBeginLoc(), "/*found*/");
    engine.Report(s->getBeginLoc(), id) << "found" << source_range << fix_it;
  }

  void onStartOfTranslationUnit() override { replacement_ = {}; }

  void onEndOfTranslationUnit() override {
    std::error_code ec;
    llvm::raw_fd_ostream file{replacement_.MainSourceFile + ".yaml", ec, llvm::sys::fs::OF_None};
    if (ec) {
      llvm::errs() << llvm::format("Error opening replacement file: %s\n", ec.message().c_str());
      return;
    }

    llvm::yaml::Output yaml{file};
    yaml << replacement_;
  }

private:
  clang::tooling::TranslationUnitReplacements replacement_;
};

} // namespace

int main(int argc, const char **argv) {
  llvm::cl::OptionCategory cat{"refactor"};
  clang::tooling::CommonOptionsParser op{argc, argv, cat};
  clang::tooling::ClangTool tool{op.getCompilations(), op.getSourcePathList()};

  MatchHandler handler{};

  clang::ast_matchers::MatchFinder finder{};
  using namespace clang::ast_matchers;
  auto m = cxxOperatorCallExpr(hasArgument(0, hasType(cxxRecordDecl(hasName("Foo")))), isAssignmentOperator());
  finder.addMatcher(m.bind("ref"), &handler);

  return tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
}
