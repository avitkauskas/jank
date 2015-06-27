#include <jank/parse/cell/stream.hpp>
#include <jank/translate/cell/stream.hpp>
#include <jank/translate/expect/type.hpp>
#include <jank/interpret/interpret.hpp>
#include <jank/interpret/environment/resolve_value.hpp>
#include <jank/interpret/expect/error/lookup/exception.hpp>

/* TODO: print with no newline. */
namespace jank
{
  namespace interpret
  {
    parse::cell::cell interpret
    (
      std::shared_ptr<environment::scope> const &env,
      translate::cell::function_body const &root
    )
    {
      for(auto const &c : root.data.cells)
      {
        switch(static_cast<translate::cell::type>(c.which()))
        {
          case translate::cell::type::function_call:
          {
            auto const &cell
            (translate::expect::type<translate::cell::type::function_call>(c));

            auto const next_scope(std::make_shared<environment::scope>());
            next_scope->parent = env;

            auto arg_name_it(cell.data.definition.arguments.begin());
            for(auto const &arg : cell.data.arguments)
            {
              auto const &name(*arg_name_it++);
              auto const var(resolve_value(next_scope, arg.cell));
              std::cout << "adding " << name.name
                        << " to next scope as: " << var << std::endl;
              next_scope->variables[name.name] = var;
            }

            std::cout << "call (" << cell.data.definition.name << "): ";
            for(auto const &arg : cell.data.arguments)
            { std::cout << resolve_value(env, arg.cell) << " "; }
            std::cout << std::endl;

            return interpret(next_scope, { cell.data.definition.body });
          } break;

          case translate::cell::type::return_statement:
          {
            auto const &cell
            (translate::expect::type<translate::cell::type::return_statement>(c));
            std::cout << "returning: " << cell.data.cell << std::endl;
            return resolve_value(env, cell.data.cell);
          } break;

          default:
            break;
        }
      }

      return parse::cell::null{};
    }
  }
}
