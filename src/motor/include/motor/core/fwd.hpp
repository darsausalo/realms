#ifndef MOTOR_CORE_FWD_HPP
#define MOTOR_CORE_FWD_HPP

namespace motor {

// member serialization macro
// example:
//
//      template<typename Archive>
//      void serialize(Archive& ar, position& p) {
//          ar.member(M(p.x));
//          ar.member(M(p.y));
//      }
//

#define M(member) NAMEOF(member), member


} // namespace motor

#endif // !MOTOR_CORE_FWD_HPP
