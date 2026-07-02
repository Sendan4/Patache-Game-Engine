/*
namespace Patache
{
template <typename T, std::size_t N>
void
AddQueue (T (&rArr)[N], T data)
{
  fast_io::io::println (N);

  for (std::uint32_t i{ 1U }; i < N; ++i)
    {
      if (i + 1 < N)
        rArr[i + 1] = rArr[i];

      if ((i + 2) > N)
        break;
    }

  rArr[0] = data;
}
}
*/
