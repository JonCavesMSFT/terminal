// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <d2d1.h>
#include <d3d11_1.h>
#include <dwrite_3.h>
#include <memory_resource>

#include "../../renderer/inc/IRenderEngine.hpp"

namespace Microsoft::Console::Render
{
    class AtlasEngine final : public IRenderEngine
    {
    public:
        explicit AtlasEngine();

        AtlasEngine(const AtlasEngine&) = delete;
        AtlasEngine& operator=(const AtlasEngine&) = delete;

        // IRenderEngine
        [[nodiscard]] HRESULT StartPaint() noexcept override;
        [[nodiscard]] HRESULT EndPaint() noexcept override;
        [[nodiscard]] bool RequiresContinuousRedraw() noexcept override;
        void WaitUntilCanRender() noexcept override;
        [[nodiscard]] HRESULT Present() noexcept override;
        [[nodiscard]] HRESULT PrepareForTeardown(_Out_ bool* pForcePaint) noexcept override;
        [[nodiscard]] HRESULT ScrollFrame() noexcept override;
        [[nodiscard]] HRESULT Invalidate(const til::rect* psrRegion) noexcept override;
        [[nodiscard]] HRESULT InvalidateCursor(const til::rect* psrRegion) noexcept override;
        [[nodiscard]] HRESULT InvalidateSystem(const til::rect* prcDirtyClient) noexcept override;
        [[nodiscard]] HRESULT InvalidateSelection(const std::vector<til::rect>& rectangles) noexcept override;
        [[nodiscard]] HRESULT InvalidateScroll(const til::point* pcoordDelta) noexcept override;
        [[nodiscard]] HRESULT InvalidateAll() noexcept override;
        [[nodiscard]] HRESULT InvalidateFlush(_In_ const bool circled, _Out_ bool* const pForcePaint) noexcept override;
        [[nodiscard]] HRESULT InvalidateTitle(std::wstring_view proposedTitle) noexcept override;
        [[nodiscard]] HRESULT NotifyNewText(const std::wstring_view newText) noexcept override;
        [[nodiscard]] HRESULT PrepareRenderInfo(const RenderFrameInfo& info) noexcept override;
        [[nodiscard]] HRESULT ResetLineTransform() noexcept override;
        [[nodiscard]] HRESULT PrepareLineTransform(LineRendition lineRendition, size_t targetRow, size_t viewportLeft) noexcept override;
        [[nodiscard]] HRESULT PaintBackground() noexcept override;
        [[nodiscard]] HRESULT PaintBufferLine(gsl::span<const Cluster> clusters, til::point coord, bool fTrimLeft, bool lineWrapped) noexcept override;
        [[nodiscard]] HRESULT PaintBufferGridLines(GridLineSet lines, COLORREF color, size_t cchLine, til::point coordTarget) noexcept override;
        [[nodiscard]] HRESULT PaintSelection(const til::rect& rect) noexcept override;
        [[nodiscard]] HRESULT PaintCursor(const CursorOptions& options) noexcept override;
        [[nodiscard]] HRESULT UpdateDrawingBrushes(const TextAttribute& textAttributes, const RenderSettings& renderSettings, gsl::not_null<IRenderData*> pData, bool usingSoftFont, bool isSettingDefaultBrushes) noexcept override;
        [[nodiscard]] HRESULT UpdateFont(const FontInfoDesired& FontInfoDesired, _Out_ FontInfo& FontInfo) noexcept override;
        [[nodiscard]] HRESULT UpdateSoftFont(gsl::span<const uint16_t> bitPattern, til::size cellSize, size_t centeringHint) noexcept override;
        [[nodiscard]] HRESULT UpdateDpi(int iDpi) noexcept override;
        [[nodiscard]] HRESULT UpdateViewport(const til::inclusive_rect& srNewViewport) noexcept override;
        [[nodiscard]] HRESULT GetProposedFont(const FontInfoDesired& FontInfoDesired, _Out_ FontInfo& FontInfo, int iDpi) noexcept override;
        [[nodiscard]] HRESULT GetDirtyArea(gsl::span<const til::rect>& area) noexcept override;
        [[nodiscard]] HRESULT GetFontSize(_Out_ til::size* pFontSize) noexcept override;
        [[nodiscard]] HRESULT IsGlyphWideByFont(std::wstring_view glyph, _Out_ bool* pResult) noexcept override;
        [[nodiscard]] HRESULT UpdateTitle(std::wstring_view newTitle) noexcept override;

        // DxRenderer - getter
        HRESULT Enable() noexcept override;
        [[nodiscard]] bool GetRetroTerminalEffect() const noexcept override;
        [[nodiscard]] float GetScaling() const noexcept override;
        [[nodiscard]] HANDLE GetSwapChainHandle() override;
        [[nodiscard]] Types::Viewport GetViewportInCharacters(const Types::Viewport& viewInPixels) const noexcept override;
        [[nodiscard]] Types::Viewport GetViewportInPixels(const Types::Viewport& viewInCharacters) const noexcept override;
        // DxRenderer - setter
        void SetAntialiasingMode(D2D1_TEXT_ANTIALIAS_MODE antialiasingMode) noexcept override;
        void SetCallback(std::function<void()> pfn) noexcept override;
        void EnableTransparentBackground(const bool isTransparent) noexcept override;
        void SetForceFullRepaintRendering(bool enable) noexcept override;
        [[nodiscard]] HRESULT SetHwnd(HWND hwnd) noexcept override;
        void SetPixelShaderPath(std::wstring_view value) noexcept override;
        void SetRetroTerminalEffect(bool enable) noexcept override;
        void SetSelectionBackground(COLORREF color, float alpha = 0.5f) noexcept override;
        void SetSoftwareRendering(bool enable) noexcept override;
        void SetWarningCallback(std::function<void(HRESULT)> pfn) noexcept override;
        [[nodiscard]] HRESULT SetWindowSize(til::size pixels) noexcept override;
        void ToggleShaderEffects() noexcept override;
        [[nodiscard]] HRESULT UpdateFont(const FontInfoDesired& pfiFontInfoDesired, FontInfo& fiFontInfo, const std::unordered_map<std::wstring_view, uint32_t>& features, const std::unordered_map<std::wstring_view, float>& axes) noexcept override;
        void UpdateHyperlinkHoveredId(uint16_t hoveredId) noexcept override;

        // Some helper classes for the implementation.
        // public because I don't want to sprinkle the code with friends.
    public:
#define ATLAS_POD_OPS(type)                                    \
    constexpr bool operator==(const type& rhs) const noexcept  \
    {                                                          \
        return __builtin_memcmp(this, &rhs, sizeof(rhs)) == 0; \
    }                                                          \
                                                               \
    constexpr bool operator!=(const type& rhs) const noexcept  \
    {                                                          \
        return __builtin_memcmp(this, &rhs, sizeof(rhs)) != 0; \
    }

#define ATLAS_FLAG_OPS(type, underlying)                                                                                                                    \
    friend constexpr type operator~(type v) noexcept { return static_cast<type>(~static_cast<underlying>(v)); }                                             \
    friend constexpr type operator|(type lhs, type rhs) noexcept { return static_cast<type>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs)); } \
    friend constexpr type operator&(type lhs, type rhs) noexcept { return static_cast<type>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs)); } \
    friend constexpr void operator|=(type& lhs, type rhs) noexcept { lhs = lhs | rhs; }                                                                     \
    friend constexpr void operator&=(type& lhs, type rhs) noexcept { lhs = lhs & rhs; }

        template<typename T>
        struct vec2
        {
            T x{};
            T y{};

            ATLAS_POD_OPS(vec2)
        };

        template<typename T>
        struct vec4
        {
            T x{};
            T y{};
            T z{};
            T w{};

            ATLAS_POD_OPS(vec4)
        };

        template<typename T>
        struct rect
        {
            T left{};
            T top{};
            T right{};
            T bottom{};

            ATLAS_POD_OPS(rect)

            constexpr bool non_empty() noexcept
            {
                return (left < right) & (top < bottom);
            }
        };

        using u8 = uint8_t;

        using u16 = uint16_t;
        using u16x2 = vec2<u16>;
        using u16r = rect<u16>;

        using i16 = int16_t;

        using u32 = uint32_t;
        using u32x2 = vec2<u32>;

        using i32 = int32_t;

        using u64 = uint64_t;

        using f32 = float;
        using f32x2 = vec2<f32>;
        using f32x4 = vec4<f32>;

        struct TextAnalyzerResult
        {
            u32 textPosition = 0;
            u32 textLength = 0;

            // These 2 fields represent DWRITE_SCRIPT_ANALYSIS.
            // Not using DWRITE_SCRIPT_ANALYSIS drops the struct size from 20 down to 12 bytes.
            u16 script = 0;
            u8 shapes = 0;

            u8 bidiLevel = 0;
        };

    private:
        template<typename T, size_t Alignment = alignof(T)>
        struct Buffer
        {
            constexpr Buffer() noexcept = default;

            explicit Buffer(size_t size) :
                _data{ allocate(size) },
                _size{ size }
            {
                std::uninitialized_default_construct_n(_data, size);
            }

            Buffer(const T* data, size_t size) :
                _data{ allocate(size) },
                _size{ size }
            {
                // Changing the constructor arguments to accept std::span might
                // be a good future extension, but not to improve security here.
                // You can trivially construct std::span's from invalid ranges.
                // Until then the raw-pointer style is more practical.
#pragma warning(suppress : 26459) // You called an STL function '...' with a raw pointer parameter at position '3' that may be unsafe [...].
                std::uninitialized_copy_n(data, size, _data);
            }

            ~Buffer()
            {
                destroy();
            }

            Buffer(Buffer&& other) noexcept :
                _data{ std::exchange(other._data, nullptr) },
                _size{ std::exchange(other._size, 0) }
            {
            }

#pragma warning(suppress : 26432) // If you define or delete any default operation in the type '...', define or delete them all (c.21).
            Buffer& operator=(Buffer&& other) noexcept
            {
                destroy();
                _data = std::exchange(other._data, nullptr);
                _size = std::exchange(other._size, 0);
                return *this;
            }

            explicit operator bool() const noexcept
            {
                return _data != nullptr;
            }

            T& operator[](size_t index) noexcept
            {
                assert(index < _size);
                return _data[index];
            }

            const T& operator[](size_t index) const noexcept
            {
                assert(index < _size);
                return _data[index];
            }

            T* data() noexcept
            {
                return _data;
            }

            const T* data() const noexcept
            {
                return _data;
            }

            size_t size() const noexcept
            {
                return _size;
            }

            T* begin() noexcept
            {
                return _data;
            }

            T* begin() const noexcept
            {
                return _data;
            }

            T* end() noexcept
            {
                return _data + _size;
            }

            T* end() const noexcept
            {
                return _data + _size;
            }

        private:
            // These two functions don't need to use scoped objects or standard allocators,
            // since this class is in fact an scoped allocator object itself.
#pragma warning(push)
#pragma warning(disable : 26402) // Return a scoped object instead of a heap-allocated if it has a move constructor (r.3).
#pragma warning(disable : 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
            static T* allocate(size_t size)
            {
                if constexpr (Alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
                {
                    return static_cast<T*>(::operator new(size * sizeof(T)));
                }
                else
                {
                    return static_cast<T*>(::operator new(size * sizeof(T), static_cast<std::align_val_t>(Alignment)));
                }
            }

            static void deallocate(T* data) noexcept
            {
                if constexpr (Alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
                {
                    ::operator delete(data);
                }
                else
                {
                    ::operator delete(data, static_cast<std::align_val_t>(Alignment));
                }
            }
#pragma warning(pop)

            void destroy() noexcept
            {
                std::destroy_n(_data, _size);
                deallocate(_data);
            }

            T* _data = nullptr;
            size_t _size = 0;
        };

        struct FontMetrics
        {
            wil::com_ptr<IDWriteFontCollection> fontCollection;
            wil::unique_process_heap_string fontName;
            float baselineInDIP = 0.0f;
            float fontSizeInDIP = 0.0f;
            u16x2 cellSize;
            u16 fontWeight = 0;
            u16 underlinePos = 0;
            u16 strikethroughPos = 0;
            u16 lineThickness = 0;
        };

        // These flags are shared with shader_ps.hlsl.
        // If you change this be sure to copy it over to shader_ps.hlsl.
        //
        // clang-format off
        enum class CellFlags : u32
        {
            None            = 0x00000000,
            HeapdKey        = 0x00000001,
            HeapdCoords     = 0x00000002,

            ColoredGlyph    = 0x00000004,

            Cursor          = 0x00000008,
            Selected        = 0x00000010,
            BorderLeft      = 0x00000020,
            BorderTop       = 0x00000040,
            BorderRight     = 0x00000080,
            BorderBottom    = 0x00000100,
            Underline       = 0x00000200,
            UnderlineDotted = 0x00000400,
            UnderlineDouble = 0x00000800,
            Strikethrough   = 0x00001000,
        };
        // clang-format on
        ATLAS_FLAG_OPS(CellFlags, u32)

        // This structure is shared with the GPU shader and needs to follow certain alignment rules.
        // You can generally assume that only u32 or types of that alignment are allowed.
        struct Cell
        {
            alignas(u32) u16x2 tileIndex;
            alignas(u32) CellFlags flags = CellFlags::None;
            u32x2 color;
        };

        enum class AtlasEntryKeyAttributes : u16
        {
            None = 0x0,
            Intense = 0x1,
            Italic = 0x2,
            // The Intense and Italic flags are used to directly index into arrays.
            // If you ever add more flags here, make sure to fix _getTextFormat()
            // and _getTextFormatAxis() and to add a `& 3` mask for instance.
        };
        ATLAS_FLAG_OPS(AtlasEntryKeyAttributes, u16)

        // AtlasEntryKey will be hashed as a series of u32 values.
        struct alignas(u32) AtlasEntryKey
        {
            AtlasEntryKeyAttributes attributes;
            u16 charCount;
            u16 coordCount;
            wchar_t chars[1];
        };

        struct alignas(u32) AtlasEntryValue
        {
            CellFlags flags = CellFlags::None;
            u16x2 coords[1];
        };

        struct AtlasEntry
        {
            AtlasEntry* next;
            AtlasEntry* prev;

            AtlasEntry* newer;
            AtlasEntry* older;

            AtlasEntryValue* value;

            u32 allocSize;
            u32 hash;
            AtlasEntryKey key;
        };

        struct unsynchronized_pool_resource
        {
            // This is a move-only type
            unsynchronized_pool_resource() = default;
            unsynchronized_pool_resource(const unsynchronized_pool_resource&) = delete;
            unsynchronized_pool_resource& operator=(const unsynchronized_pool_resource&) = delete;
            unsynchronized_pool_resource(unsynchronized_pool_resource&&) = default;
            unsynchronized_pool_resource& operator=(unsynchronized_pool_resource&&) = default;

            ~unsynchronized_pool_resource() noexcept
            {
                release();
            }

            _NODISCARD __declspec(allocator) void* allocate(_CRT_GUARDOVERFLOW const size_t _Bytes, const size_t _Align = alignof(max_align_t))
            { // allocate _Bytes bytes of memory with alignment _Align
                _STL_ASSERT(std::_Is_pow_2(_Align), "memory_resource::allocate(): Alignment must be a power of two.");
                void* _Ptr = do_allocate(_Bytes, _Align);
                return ::operator new(_Bytes, _Ptr);
            }

            void deallocate(void* const _Ptr, const size_t _Bytes, const size_t _Align = alignof(max_align_t))
            {
                // deallocate _Ptr, which was returned from allocate(_Bytes, _Align)
                _STL_ASSERT(std::_Is_pow_2(_Align), "memory_resource::deallocate(): Alignment must be a power of two.");
                return do_deallocate(_Ptr, _Bytes, _Align);
            }

            void release() noexcept /* strengthened */
            {
                // release all allocations back upstream
                for (auto& _Al : _Pools)
                {
                    _Al._Clear();
                }
                _Pools.clear();
                _Pools.shrink_to_fit();
            }

        protected:
            void* do_allocate(size_t _Bytes, const size_t _Align)
            {
                auto _Result = _Find_pool(_Bytes, _Align);
                if (_Result.first == _Pools.end() || _Result.first->_Log_of_size != _Result.second)
                {
                    _Result.first = _Pools.emplace(_Result.first, _Result.second);
                }

                return _Result.first->_Allocate();
            }

            void do_deallocate(void* const _Ptr, const size_t _Bytes, const size_t _Align)
            {
                const auto _Result = _Find_pool(_Bytes, _Align);
                if (_Result.first != _Pools.end() && _Result.first->_Log_of_size == _Result.second)
                {
                    _Result.first->_Deallocate(_Ptr);
                }
            }

        private:
            // manager for a collection of chunks comprised of blocks of a single size
            struct _Pool
            {
                // a memory allocation consisting of a number of fixed-size blocks to be parceled out
                struct _Chunk : std::pmr::_Single_link<>
                {
                    std::pmr::_Intrusive_stack<_Single_link<>> _Free_blocks{}; // list of free blocks
                    size_t _Free_count; // # of unallocated blocks
                    size_t _Capacity; // total # of blocks
                    char* _Base; // address of first block
                    size_t _Next_available = 0; // index of first never-allocated block
                    size_t _Id; // unique identifier; increasing order of allocation

                    // initialize a chunk of _Capacity blocks, all initially free
                    _Chunk(_Pool& _Al, void* const _Base_, const size_t _Capacity_) noexcept :
                        _Free_count{ _Capacity_ },
                        _Capacity{ _Capacity_ },
                        _Base{ static_cast<char*>(_Base_) },
                        _Id{ _Al._All_chunks._Empty() ? 0 : _Al._All_chunks._Top()->_Id + 1 }
                    {
                    }

                    _Chunk(const _Chunk&) = delete;
                    _Chunk& operator=(const _Chunk&) = delete;
                };

                _Chunk* _Unfull_chunk = nullptr; // largest _Chunk with free blocks
                std::pmr::_Intrusive_stack<_Chunk> _All_chunks{}; // all chunks (ordered by decreasing _Id)
                size_t _Next_capacity = _Default_next_capacity; // # of blocks to allocate in next _Chunk
                    // in (1, (PTRDIFF_MAX - sizeof(_Chunk)) >> _Log_of_size]
                size_t _Block_size; // size of allocated blocks
                size_t _Log_of_size; // _Block_size == 1 << _Log_of_size
                _Chunk* _Empty_chunk = nullptr; // only _Chunk with all free blocks

                static constexpr size_t _Default_next_capacity = 4;
                static_assert(_Default_next_capacity > 1);

                explicit _Pool(const size_t _Log_of_size_) noexcept :
                    _Block_size{ size_t{ 1 } << _Log_of_size_ },
                    _Log_of_size{ _Log_of_size_ }
                {
                }

                _Pool(_Pool&& _That) noexcept :
                    _Unfull_chunk{ _STD exchange(_That._Unfull_chunk, nullptr) },
                    _All_chunks{ _STD move(_That._All_chunks) },
                    _Next_capacity{ _STD exchange(_That._Next_capacity, _Default_next_capacity) },
                    _Block_size{ _That._Block_size },
                    _Log_of_size{ _That._Log_of_size },
                    _Empty_chunk{ _STD exchange(_That._Empty_chunk, nullptr) }
                {
                }

                _Pool& operator=(_Pool&& _That) noexcept
                {
                    _Unfull_chunk = _STD exchange(_That._Unfull_chunk, nullptr);
                    _All_chunks = _STD move(_That._All_chunks);
                    _Next_capacity = _STD exchange(_That._Next_capacity, _Default_next_capacity);
                    _Block_size = _That._Block_size;
                    _Log_of_size = _That._Log_of_size;
                    _Empty_chunk = _STD exchange(_That._Empty_chunk, nullptr);
                    return *this;
                }

                void _Clear() noexcept
                {
                    // release all chunks in the pool back upstream
                    std::pmr::_Intrusive_stack<_Chunk> _Tmp{};
                    _STD swap(_Tmp, _All_chunks);
                    std::pmr::memory_resource* const _Resource = std::pmr::get_default_resource();
                    while (!_Tmp._Empty())
                    {
                        const auto _Ptr = _Tmp._Pop();
                        _Resource->deallocate(_Ptr->_Base, _Size_for_capacity(_Ptr->_Capacity), _Block_size);
                    }

                    _Unfull_chunk = nullptr;
                    _Next_capacity = _Default_next_capacity;
                    _Empty_chunk = nullptr;
                }

                void* _Allocate()
                { // allocate a block from this pool
                    for (;; _Unfull_chunk = _All_chunks._As_item(_Unfull_chunk->_Next))
                    {
                        if (!_Unfull_chunk)
                        {
                            _Increase_capacity();
                        }
                        else if (!_Unfull_chunk->_Free_blocks._Empty())
                        {
                            if (_Unfull_chunk == _Empty_chunk)
                            { // this chunk is no longer empty
                                _Empty_chunk = nullptr;
                            }
                            --_Unfull_chunk->_Free_count;
                            return _Unfull_chunk->_Free_blocks._Pop();
                        }

                        if (_Unfull_chunk->_Next_available < _Unfull_chunk->_Capacity)
                        {
                            if (_Unfull_chunk == _Empty_chunk)
                            { // this chunk is no longer empty
                                _Empty_chunk = nullptr;
                            }
                            --_Unfull_chunk->_Free_count;
                            char* const _Block = _Unfull_chunk->_Base + _Unfull_chunk->_Next_available * _Block_size;
                            ++_Unfull_chunk->_Next_available;
                            *(reinterpret_cast<_Chunk**>(_Block + _Block_size) - 1) = _Unfull_chunk;
                            return _Block;
                        }
                    }
                }

                void _Deallocate(void* const _Ptr) noexcept
                {
                    // return a block to this pool
                    _Chunk* _Current = *(reinterpret_cast<_Chunk**>(static_cast<char*>(_Ptr) + _Block_size) - 1);

                    _Current->_Free_blocks._Push(::new (_Ptr) std::pmr::_Single_link<>);

                    if (_Current->_Free_count++ == 0)
                    {
                        // prefer to allocate from newer/larger chunks...
                        if (!_Unfull_chunk || _Unfull_chunk->_Id < _Current->_Id)
                        {
                            _Unfull_chunk = _Current;
                        }

                        return;
                    }

                    if (_Current->_Free_count < _Current->_Capacity)
                    {
                        return;
                    }

                    if (!_Empty_chunk)
                    {
                        _Empty_chunk = _Current;
                        return;
                    }

                    // ...and release older/smaller chunks to keep the list lengths short.
                    if (_Empty_chunk->_Id < _Current->_Id)
                    {
                        _STD swap(_Current, _Empty_chunk);
                    }

                    _All_chunks._Remove(_Current);
                    std::pmr::get_default_resource()->deallocate(
                        _Current->_Base, _Size_for_capacity(_Current->_Capacity), _Block_size);
                }

                size_t _Size_for_capacity(const size_t _Capacity) const noexcept
                {
                    // return the size of a chunk that holds _Capacity blocks
                    return (_Capacity << _Log_of_size) + sizeof(_Chunk);
                }

                void _Increase_capacity()
                {
                    // this pool has no free blocks; get a new chunk from upstream
                    const size_t _Size = _Size_for_capacity(_Next_capacity);
                    std::pmr::memory_resource* const _Resource = std::pmr::get_default_resource();
                    void* const _Ptr = _Resource->allocate(_Size, _Block_size);
                    std::pmr::_Check_alignment(_Ptr, _Block_size);

                    void* const _Tmp = static_cast<char*>(_Ptr) + _Size - sizeof(_Chunk);
                    _Unfull_chunk = ::new (_Tmp) _Chunk{ *this, _Ptr, _Next_capacity };
                    _Empty_chunk = _Unfull_chunk;
                    _All_chunks._Push(_Unfull_chunk);

                    // scale _Next_capacity by 2, saturating so that _Size_for_capacity(_Next_capacity) cannot overflow
                    _Next_capacity = std::min(_Next_capacity << 1, (PTRDIFF_MAX - sizeof(_Chunk)) >> _Log_of_size);
                }
            };

            std::pair<std::pmr::vector<_Pool>::iterator, unsigned char> _Find_pool(
                const size_t _Bytes,
                const size_t _Align) noexcept
            {
                // find the pool from which to allocate a block with size _Bytes and alignment _Align
                const size_t _Size = std::max(_Bytes + sizeof(void*), _Align);
                const auto _Log_of_size = static_cast<unsigned char>(std::_Ceiling_of_log_2(_Size));
                return { _STD lower_bound(_Pools.begin(), _Pools.end(), _Log_of_size, [](const _Pool& _Al, const unsigned char _Log) { return _Al._Log_of_size < _Log; }), _Log_of_size };
            }

            std::pmr::vector<_Pool> _Pools;
        };

        struct TileHashMap
        {
            TileHashMap() = default;

            AtlasEntry* findOrInsert(AtlasEntryKeyAttributes attributes, u16 charCount, u16 coordCount, const wchar_t* chars, bool& inserted)
            {
                const auto keySize = nextMultipleOf(calculateKeySize(charCount), sizeof(u32));
                const auto key = static_cast<AtlasEntryKey*>(_allocator.allocate(keySize, alignof(AtlasEntryKey)));
                u32 hash;

                key->attributes = attributes;
                key->charCount = charCount;
                key->coordCount = coordCount;
#pragma warning(suppress : 26490) // Don't use reinterpret_cast (type.1).
                {
                    // totalSize is rounded up to the next multiple of 4, but
                    // charCount might only amount to a multiple of 2 (bytes).
                    // memset()ing the last wchar_t ensures we don't hash uninitialized data.
                    const auto data = reinterpret_cast<u8*>(key);
                    memset(data + keySize - 2, 0, 2);

                    // This will potentially overwrite the memset()'d wchar_t above.
                    std::copy_n(chars, charCount, &key->chars[0]);

                    hash = hashData(data, keySize);
                }

                {
                    const auto it = _map[hash & _mapMask];
                    for (auto entry = it; entry; entry = entry->next)
                    {
                        const auto itKeySize = til::bit_cast<uintptr_t>(entry->value) - til::bit_cast<uintptr_t>(&entry->key);
                        if (itKeySize == keySize && memcmp(key, &entry->key, keySize) == 0)
                        {
                            _allocator.deallocate(key, keySize, alignof(AtlasEntryKey));

                            // LRU remove
                            lruRemove(entry);
                            lruPush(entry);

                            return entry;
                        }
                    }
                }

                if (_size == _mapSize)
                {
                    const auto newMapSize = _mapSize << 1;
                    const auto newMapMask = newMapSize - 1;
                    FAIL_FAST_IF(newMapSize <= _mapSize); // overflow
                    auto newMap = std::make_unique<AtlasEntry*[]>(newMapSize);

                    auto it = _map.get();
                    const auto end = it + _mapSize;
                    for (; it != end; ++it)
                    {
                        for (auto entry = *it; entry;)
                        {
                            const auto next = entry->next;
                            mapPush(&newMap[entry->hash & newMapMask], entry);
                            entry = next;
                        }
                    }

                    _map = std::move(newMap);
                    _mapSize = newMapSize;
                    _mapMask = newMapMask;
                }

                const auto valueOffset = offsetof(AtlasEntry, key) + keySize;
                const auto totalSize = valueOffset + calculateValueSize(coordCount);
                const auto entry = static_cast<AtlasEntry*>(_allocator.allocate(totalSize, alignof(AtlasEntry)));
                const auto slot = &_map[hash & _mapMask];

                entry->value = til::bit_cast<AtlasEntryValue*>(til::bit_cast<uintptr_t>(entry) + valueOffset);
                entry->allocSize = gsl::narrow_cast<u32>(totalSize);
                entry->hash = hash;
                memcpy(&entry->key, key, keySize);

                mapPush(slot, entry);
                lruPush(entry);

                _size++;

                inserted = true;
                return entry;
            }

            void popOldestTiles(std::vector<u16x2>& out)
            {
                FAIL_FAST_IF(!_size || !_oldest);

                const auto entry = _oldest;

                if (!entry->newer)
                {
                    __debugbreak();
                }

                const auto slot = &_map[entry->hash & _mapMask];
                const auto offset = out.size();
                const auto cellCount = entry->key.coordCount;
                out.resize(offset + cellCount);
                std::copy_n(&entry->value->coords[0], cellCount, out.begin() + offset);

                mapRemove(slot, entry);
                lruRemove(entry);

                auto it = std::find(_map.get(), _map.get() + _mapSize, entry);
                if (it != (_map.get() + _mapSize))
                {
                    __debugbreak();
                }

                _size--;

                _allocator.deallocate(entry, entry->allocSize, alignof(AtlasEntry));
            }

            void reset() noexcept
            {
                _allocator.release();
                memset(_map.get(), 0, _mapSize * sizeof(_map[0]));
                _newest = nullptr;
                _oldest = nullptr;
                _size = 0;
            }

        private:
            static constexpr size_t calculateKeySize(size_t charCount) noexcept
            {
                return sizeof(AtlasEntryKey) - sizeof(AtlasEntryKey::chars) + charCount * sizeof(AtlasEntryKey::chars[0]);
            }

            static constexpr size_t calculateValueSize(size_t coordCount) noexcept
            {
                return sizeof(AtlasEntryValue) - sizeof(AtlasEntryValue::coords) + coordCount * sizeof(AtlasEntryValue::coords[0]);
            }

            static constexpr size_t nextMultipleOf(size_t n, size_t powerOf2) noexcept
            {
                return (n + powerOf2 - 1) & ~(powerOf2 - 1);
            }

            static u32 hashData(const u8* beg, size_t length) noexcept
            {
                // This hash function only works with data fully aligned to u32 (including the length).
                assert(til::bit_cast<uintptr_t>(beg) % sizeof(u32) == 0);
                assert(length % sizeof(u32) == 0);

                const auto end = beg + length;

                // This loop is a simple LCG (linear congruential generator) with Donald Knuth's
                // widely used parameters. Unlike with normal LCGs however we mix in
                // 4 bytes of the input on each iteration using a simple XOR.
                auto h = UINT64_C(0x243F6A8885A308D3); // fractional digits of pi in hex (OEIS: A062964)
                for (; beg != end; beg += sizeof(u32))
                {
                    // Neither x64 nor ARM64 assembly differentiates between aligned and unaligned loads.
                    // As such we can freely use the standard compliant way of reading u8*: memcpy().
                    // (In Release mode this should be inlined to a single instruction.)
                    u32 v;
                    memcpy(&v, beg, sizeof(u32));
                    h = (h ^ v) * UINT64_C(6364136223846793005) + UINT64_C(1442695040888963407);
                }

                // PCG (permuted congruential generator) XSL-RR finalizer.
                // In testing it seemed sufficient for the purpose of a hash-map key generator.
                //
                // Copyright 2014-2017 Melissa O'Neill <oneill@pcg-random.org>, and the PCG Project contributors.
                // See oss/pcg/LICENSE-MIT.txt, oss/pcg/LICENSE-APACHE.txt or https://www.pcg-random.org/.
                const int r = h & 63;
                const auto x = gsl::narrow_cast<u32>(h >> 32) ^ gsl::narrow_cast<u32>(h);
                return _rotl(x, r);
            }

            void lruRemove(const AtlasEntry* entry) noexcept
            {
                if (entry->newer)
                {
                    entry->newer->older = entry->older;
                }
                else
                {
                    _newest = entry->older;
                }
                if (entry->older)
                {
                    entry->older->newer = entry->newer;
                }
                else
                {
                    _oldest = entry->newer;
                }
            }

            void lruPush(AtlasEntry* entry) noexcept
            {
                if (_newest)
                {
                    _newest->newer = entry;
                }
                else
                {
                    _oldest = entry;
                }
                entry->newer = nullptr;
                entry->older = _newest;
                _newest = entry;
            }

            static void mapRemove(AtlasEntry** slot, const AtlasEntry* entry) noexcept
            {
                if (entry->prev)
                {
                    entry->prev->next = entry->next;
                }
                else
                {
                    *slot = entry->next;
                }
                if (entry->next)
                {
                    entry->next->prev = entry->prev;
                }
            }

            static void mapPush(AtlasEntry** slot, AtlasEntry* entry) noexcept
            {
                if (*slot)
                {
                    (*slot)->prev = entry;
                }
                entry->next = *slot;
                entry->prev = nullptr;
                *slot = entry;
            }

            static constexpr u32 initialSize = 4;

            unsynchronized_pool_resource _allocator;
            std::unique_ptr<AtlasEntry*[]> _map = std::make_unique<AtlasEntry*[]>(initialSize);
            AtlasEntry* _newest = nullptr;
            AtlasEntry* _oldest = nullptr;
            u32 _mapSize = initialSize;
            u32 _mapMask = initialSize - 1;
            u32 _size = 0;
        };

        // TileAllocator yields `tileSize`-sized tiles for our texture atlas.
        // While doing so it'll grow the atlas size() by a factor of 2 if needed.
        // Once the setMaxArea() is exceeded it'll stop growing and instead
        // snatch tiles back from the oldest TileHashMap entries.
        //
        // The quadratic growth works by alternating the size()
        // between an 1:1 and 2:1 aspect ratio, like so:
        //   (64,64) -> (128,64) -> (128,128) -> (256,128) -> (256,256)
        // These initial tile positions allocate() returns are in a Z
        // pattern over the available space in the atlas texture.
        // You can log the `return _pos;` in allocate() using "Tracepoint"s
        // in Visual Studio if you'd like to understand the Z pattern better.
        struct TileAllocator
        {
            TileAllocator() = default;

            explicit TileAllocator(u16x2 tileSize, u16x2 windowSize) noexcept :
                _tileSize{ tileSize }
            {
                const auto initialSize = std::max(u16{ _absoluteMinSize }, std::bit_ceil(std::max(tileSize.x, tileSize.y)));
                _size = { initialSize, initialSize };
                _limit = { gsl::narrow_cast<u16>(initialSize - _tileSize.x), gsl::narrow_cast<u16>(initialSize - _tileSize.y) };
                setMaxArea(windowSize);
            }

            u16x2 size() const noexcept
            {
                return _size;
            }

            void setMaxArea(u16x2 windowSize) noexcept
            {
                // _generate() uses a quadratic growth factor for _size's area.
                // Once it exceeds the _maxArea, it'll start snatching tiles back from the
                // TileHashMap using its LRU queue. Since _size will at least reach half
                // of _maxSize (because otherwise it could still grow by a factor of 2)
                // and by ensuring that _maxArea is at least twice the window size
                // we make it impossible* for _generate() to return false before
                // TileHashMap contains at least as many tiles as the window contains.
                // If that wasn't the case we'd snatch and reuse tiles that are still in use.
                // * lhecker's legal department:
                //   No responsibility is taken for the correctness of this information.
                setMaxArea(static_cast<size_t>(windowSize.x) * static_cast<size_t>(windowSize.y) * 2);
            }

            void setMaxArea(size_t max) noexcept
            {
                // We need to reserve at least 1 extra `tileArea`, because the tile
                // at position {0,0} is already reserved for the cursor texture.
                const auto tileArea = static_cast<size_t>(_tileSize.x) * static_cast<size_t>(_tileSize.y);
                _maxArea = clamp(max + tileArea, _absoluteMinArea, _absoluteMaxArea);
                _updateCanGenerate();
            }

            u16x2 allocate(TileHashMap& map) noexcept
            {
                if (_generate())
                {
                    return _pos;
                }

                if (_cache.empty())
                {
                    map.popOldestTiles(_cache);
                }

                const auto pos = _cache.back();
                _cache.pop_back();
                return pos;
            }

            void reset() noexcept
            {
                _cache.clear();
                _pos = {};
                _originX = 0;
                _canGenerate = true;
            }

        private:
            // This method generates the Z pattern coordinates
            // described above in the TileAllocator comment.
            bool _generate() noexcept
            {
                if (!_canGenerate)
                {
                    return false;
                }

                // We need to backup _pos/_size in case our resize below exceeds _maxArea.
                // In that case we have to restore _pos/_size so that if _maxArea is increased
                // (window resize for instance), we can pick up where we previously left off.
                const auto pos = _pos;

                _pos.x += _tileSize.x;
                if (_pos.x <= _limit.x)
                {
                    return true;
                }

                _pos.y += _tileSize.y;
                if (_pos.y <= _limit.y)
                {
                    _pos.x = _originX;
                    return true;
                }

                // Same as for pos.
                const auto size = _size;

                // This implements a quadratic growth factor for _size, by
                // alternating between an 1:1 and 2:1 aspect ratio, like so:
                //   (64,64) -> (128,64) -> (128,128) -> (256,128) -> (256,256)
                // This behavior is strictly dependent on setMaxArea(u16x2)'s
                // behavior. See it's comment for an explanation.
                if (_size.x == _size.y)
                {
                    _size.x *= 2;
                    _pos.y = 0;
                }
                else
                {
                    _size.y *= 2;
                    _pos.x = 0;
                }

                _updateCanGenerate();
                if (_canGenerate)
                {
                    _limit = { gsl::narrow_cast<u16>(_size.x - _tileSize.x), gsl::narrow_cast<u16>(_size.y - _tileSize.y) };
                    _originX = _pos.x;
                }
                else
                {
                    _size = size;
                    _pos = pos;
                }

                return _canGenerate;
            }

            void _updateCanGenerate() noexcept
            {
                _canGenerate = static_cast<size_t>(_size.x) * static_cast<size_t>(_size.y) <= _maxArea;
            }

            static constexpr u16 _absoluteMinSize = 256;
            static constexpr size_t _absoluteMinArea = _absoluteMinSize * _absoluteMinSize;
            // TODO: Consider using IDXGIAdapter3::QueryVideoMemoryInfo() and IDXGIAdapter3::RegisterVideoMemoryBudgetChangeNotificationEvent()
            // That way we can make better to use of a user's available video memory.
            static constexpr size_t _absoluteMaxArea = D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION * D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;

            std::vector<u16x2> _cache;
            size_t _maxArea = _absoluteMaxArea;
            u16x2 _tileSize;
            u16x2 _size;
            u16x2 _limit;
            // Since _pos starts at {0, 0}, it'll result in the first allocate()d tile to be at {_tileSize.x, 0}.
            // Coincidentially that's exactly what we want as the cursor texture lives at {0, 0}.
            u16x2 _pos;
            u16 _originX = 0;
            // Indicates whether we've exhausted our Z pattern across the atlas texture.
            // If this is false, we have to snatch tiles back from TileHashMap.
            bool _canGenerate = true;
        };

        struct CachedCursorOptions
        {
            u32 cursorColor = INVALID_COLOR;
            u16 cursorType = gsl::narrow_cast<u16>(CursorType::Legacy);
            u8 heightPercentage = 20;

            ATLAS_POD_OPS(CachedCursorOptions)
        };

        struct BufferLineMetadata
        {
            u32x2 colors;
            CellFlags flags = CellFlags::None;
        };

        // NOTE: D3D constant buffers sizes must be a multiple of 16 bytes.
        struct alignas(16) ConstBuffer
        {
            // WARNING: Modify this carefully after understanding how HLSL struct packing works.
            // The gist is:
            // * Minimum alignment is 4 bytes (like `#pragma pack 4`)
            // * Members cannot straddle 16 byte boundaries
            //   This means a structure like {u32; u32; u32; u32x2} would require
            //   padding so that it is {u32; u32; u32; <4 byte padding>; u32x2}.
            // * bool will probably not work the way you want it to,
            //   because HLSL uses 32-bit bools and C++ doesn't.
            alignas(sizeof(f32x4)) f32x4 viewport;
            alignas(sizeof(f32x4)) f32 gammaRatios[4]{};
            alignas(sizeof(f32)) f32 enhancedContrast = 0;
            alignas(sizeof(u32)) u32 cellCountX = 0;
            alignas(sizeof(u32x2)) u32x2 cellSize;
            alignas(sizeof(u32x2)) u32x2 underlinePos;
            alignas(sizeof(u32x2)) u32x2 strikethroughPos;
            alignas(sizeof(u32)) u32 backgroundColor = 0;
            alignas(sizeof(u32)) u32 cursorColor = 0;
            alignas(sizeof(u32)) u32 selectionColor = 0;
            alignas(sizeof(u32)) u32 useClearType = 0;
#pragma warning(suppress : 4324) // 'ConstBuffer': structure was padded due to alignment specifier
        };

        // Handled in BeginPaint()
        enum class ApiInvalidations : u8
        {
            None = 0,
            Title = 1 << 0,
            Device = 1 << 1,
            SwapChain = 1 << 2,
            Size = 1 << 3,
            Font = 1 << 4,
            Settings = 1 << 5,
        };
        ATLAS_FLAG_OPS(ApiInvalidations, u8)

        // Handled in Present()
        enum class RenderInvalidations : u8
        {
            None = 0,
            Cursor = 1 << 0,
            ConstBuffer = 1 << 1,
        };
        ATLAS_FLAG_OPS(RenderInvalidations, u8)

        // MSVC STL (version 22000) implements std::clamp<T>(T, T, T) in terms of the generic
        // std::clamp<T, Predicate>(T, T, T, Predicate) with std::less{} as the argument,
        // which introduces branching. While not perfect, this is still better than std::clamp.
        template<typename T>
        static constexpr T clamp(T val, T min, T max)
        {
            return std::max(min, std::min(max, val));
        }

        // AtlasEngine.cpp
        [[nodiscard]] HRESULT _handleException(const wil::ResultException& exception) noexcept;
        __declspec(noinline) void _createResources();
        void _releaseSwapChain();
        __declspec(noinline) void _createSwapChain();
        __declspec(noinline) void _recreateSizeDependentResources();
        __declspec(noinline) void _recreateFontDependentResources();
        IDWriteTextFormat* _getTextFormat(AtlasEntryKeyAttributes attributes) const noexcept;
        const Buffer<DWRITE_FONT_AXIS_VALUE>& _getTextFormatAxis(AtlasEntryKeyAttributes attributes) const noexcept;
        Cell* _getCell(u16 x, u16 y) noexcept;
        const AtlasEntry** _getCellGlyphMapping(u16 x, u16 y) noexcept;
        void _setCellFlags(u16r coords, CellFlags mask, CellFlags bits) noexcept;
        void _flushBufferLine();
        void _emplaceGlyph(IDWriteFontFace* fontFace, size_t bufferPos1, size_t bufferPos2);

        // AtlasEngine.api.cpp
        void _resolveAntialiasingMode() noexcept;
        void _updateFont(const wchar_t* faceName, const FontInfoDesired& fontInfoDesired, FontInfo& fontInfo, const std::unordered_map<std::wstring_view, uint32_t>& features, const std::unordered_map<std::wstring_view, float>& axes);
        void _resolveFontMetrics(const wchar_t* faceName, const FontInfoDesired& fontInfoDesired, FontInfo& fontInfo, FontMetrics* fontMetrics = nullptr) const;

        // AtlasEngine.r.cpp
        void _setShaderResources() const;
        void _updateConstantBuffer() const noexcept;
        void _adjustAtlasSize();
        void _processGlyphQueue();
        void _drawGlyph(const AtlasEntry& item) const;
        void _drawCursor();

        static constexpr bool debugGlyphGenerationPerformance = false;
        static constexpr bool debugGeneralPerformance = false || debugGlyphGenerationPerformance;

        static constexpr u16 u16min = 0x0000;
        static constexpr u16 u16max = 0xffff;
        static constexpr i16 i16min = -0x8000;
        static constexpr i16 i16max = 0x7fff;
        static constexpr u16r invalidatedAreaNone = { u16max, u16max, u16min, u16min };
        static constexpr u16x2 invalidatedRowsNone{ u16max, u16min };
        static constexpr u16x2 invalidatedRowsAll{ u16min, u16max };

        struct StaticResources
        {
            wil::com_ptr<ID2D1Factory> d2dFactory;
            wil::com_ptr<IDWriteFactory1> dwriteFactory;
            wil::com_ptr<IDWriteFontFallback> systemFontFallback;
            wil::com_ptr<IDWriteTextAnalyzer1> textAnalyzer;
            bool isWindows10OrGreater = true;

#ifndef NDEBUG
            std::filesystem::path sourceDirectory;
            wil::unique_folder_change_reader_nothrow sourceCodeWatcher;
            std::atomic<int64_t> sourceCodeInvalidationTime{ INT64_MAX };
#endif
        } _sr;

        struct Resources
        {
            // D3D resources
            wil::com_ptr<ID3D11Device> device;
            wil::com_ptr<ID3D11DeviceContext1> deviceContext;
            wil::com_ptr<IDXGISwapChain1> swapChain;
            wil::unique_handle frameLatencyWaitableObject;
            wil::com_ptr<ID3D11RenderTargetView> renderTargetView;
            wil::com_ptr<ID3D11VertexShader> vertexShader;
            wil::com_ptr<ID3D11PixelShader> pixelShader;
            wil::com_ptr<ID3D11Buffer> constantBuffer;
            wil::com_ptr<ID3D11Buffer> cellBuffer;
            wil::com_ptr<ID3D11ShaderResourceView> cellView;

            // D2D resources
            wil::com_ptr<ID3D11Texture2D> atlasBuffer;
            wil::com_ptr<ID3D11ShaderResourceView> atlasView;
            wil::com_ptr<ID2D1RenderTarget> d2dRenderTarget;
            wil::com_ptr<ID2D1Brush> brush;
            wil::com_ptr<IDWriteTextFormat> textFormats[4];
            Buffer<DWRITE_FONT_AXIS_VALUE> textFormatAxes[4];
            wil::com_ptr<IDWriteTypography> typography;

            Buffer<Cell, 32> cells; // invalidated by ApiInvalidations::Size
            Buffer<const AtlasEntry*> cellGlyphMapping; // invalidated by ApiInvalidations::Size
            f32x2 cellSizeDIP; // invalidated by ApiInvalidations::Font, caches _api.cellSize but in DIP
            u16x2 cellSize; // invalidated by ApiInvalidations::Font, caches _api.cellSize
            u16x2 cellCount; // invalidated by ApiInvalidations::Font|Size, caches _api.cellCount
            u16 underlinePos = 0;
            u16 strikethroughPos = 0;
            u16 lineThickness = 0;
            u16 dpi = USER_DEFAULT_SCREEN_DPI; // invalidated by ApiInvalidations::Font, caches _api.dpi
            u16x2 atlasSizeInPixel; // invalidated by ApiInvalidations::Font
            TileHashMap glyphs;
            TileAllocator tileAllocator;
            std::vector<const AtlasEntry*> glyphQueue;

            f32 gamma = 0;
            f32 cleartypeEnhancedContrast = 0;
            f32 grayscaleEnhancedContrast = 0;
            u32 backgroundColor = 0xff000000;
            u32 selectionColor = 0x7fffffff;

            CachedCursorOptions cursorOptions;
            RenderInvalidations invalidations = RenderInvalidations::None;

#ifndef NDEBUG
            // See documentation for IDXGISwapChain2::GetFrameLatencyWaitableObject method:
            // > For every frame it renders, the app should wait on this handle before starting any rendering operations.
            // > Note that this requirement includes the first frame the app renders with the swap chain.
            bool frameLatencyWaitableObjectUsed = false;
#endif
        } _r;

        struct ApiState
        {
            // This structure is loosely sorted in chunks from "very often accessed together"
            // to seldom accessed and/or usually not together.

            std::vector<wchar_t> bufferLine;
            std::vector<u16> bufferLineColumn;
            Buffer<BufferLineMetadata> bufferLineMetadata;
            std::vector<TextAnalyzerResult> analysisResults;
            Buffer<u16> clusterMap;
            Buffer<DWRITE_SHAPING_TEXT_PROPERTIES> textProps;
            Buffer<u16> glyphIndices;
            Buffer<DWRITE_SHAPING_GLYPH_PROPERTIES> glyphProps;
            Buffer<f32> glyphAdvances;
            Buffer<DWRITE_GLYPH_OFFSET> glyphOffsets;
            std::vector<DWRITE_FONT_FEATURE> fontFeatures; // changes are flagged as ApiInvalidations::Font|Size
            std::vector<DWRITE_FONT_AXIS_VALUE> fontAxisValues; // changes are flagged as ApiInvalidations::Font|Size
            FontMetrics fontMetrics; // changes are flagged as ApiInvalidations::Font|Size

            u16x2 cellCount; // caches `sizeInPixel / cellSize`
            u16x2 sizeInPixel; // changes are flagged as ApiInvalidations::Size

            // UpdateDrawingBrushes()
            u32 backgroundOpaqueMixin = 0xff000000; // changes are flagged as ApiInvalidations::Device
            u32x2 currentColor;
            AtlasEntryKeyAttributes attributes = AtlasEntryKeyAttributes::None;
            u16x2 lastPaintBufferLineCoord;
            CellFlags flags = CellFlags::None;
            // SetSelectionBackground()
            u32 selectionColor = 0x7fffffff;
            // UpdateHyperlinkHoveredId()
            u16 hyperlinkHoveredId = 0;
            bool bufferLineWasHyperlinked = false;

            // dirtyRect is a computed value based on invalidatedRows.
            til::rect dirtyRect;
            // These "invalidation" fields are reset in EndPaint()
            u16r invalidatedCursorArea = invalidatedAreaNone;
            u16x2 invalidatedRows = invalidatedRowsNone; // x is treated as "top" and y as "bottom"
            i16 scrollOffset = 0;

            std::function<void(HRESULT)> warningCallback;
            std::function<void()> swapChainChangedCallback;
            wil::unique_handle swapChainHandle;
            HWND hwnd = nullptr;
            u16 dpi = USER_DEFAULT_SCREEN_DPI; // changes are flagged as ApiInvalidations::Font|Size
            u8 antialiasingMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE; // changes are flagged as ApiInvalidations::Font
            u8 realizedAntialiasingMode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE; // caches antialiasingMode, depends on antialiasingMode and backgroundOpaqueMixin, see _resolveAntialiasingMode

            ApiInvalidations invalidations = ApiInvalidations::Device;
        } _api;

#undef ATLAS_POD_OPS
#undef ATLAS_FLAG_OPS
    };
}
