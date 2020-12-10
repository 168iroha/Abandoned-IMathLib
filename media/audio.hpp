#ifndef IMATHLIB_INTERFACE_AUDIO_HPP
#define IMATHLIB_INTERFACE_AUDIO_HPP
#ifdef IMATHLIB_F_MEDIA_AUDIO

#include "IMathLib/media/interface.hpp"
#include "IMathLib/utility/utility.hpp"

namespace iml {
	namespace ml {

		//オーディオ識別用ホルダ
		template <size_t N>
		struct audio_ident {};
		//オーディオクラス(audio("hoge.ogg", audio_ident<audio::se+1>())やaudio("hoge.ogg", audio_ident<audio::bgm>()))
		class audio {
			//オーディオハンドルの基底クラス
			struct holder_base {
				virtual ~holder_base() = 0 {}

				virtual	void play(size_t loop, size_t ms) = 0;		//loop回ループしてms秒のフェードで再生
				virtual	void pause() = 0;							//一時停止
				virtual void resume() = 0;							//一時停止解除
				virtual	void reset(size_t ms) = 0;					//msのフェードでリセット

				virtual void free() = 0;
			};
			//BGM
			struct holder_bgm :holder_base {
				holder_bgm(const char* str) {
					if (!(handle = Mix_LoadMUS(str)))
						controler::inst()->log_wrrite(log_output::error, Mix_GetError(), log_output::file_read_error, str);
				}
				virtual ~holder_bgm() { free(); }

				virtual	void play(size_t loop, size_t ms) {
					Mix_FadeInMusic(handle, loop, ms);
				}
				virtual	void pause() { Mix_PauseMusic(); }
				virtual void resume() { Mix_ResumeMusic(); }
				virtual	void reset(size_t ms) {
					Mix_FadeOutMusic(ms);
					Mix_RewindMusic();
				}

				virtual void free() {
					Mix_FreeMusic(handle);
					handle = nullptr;
				}
			private:
				Mix_Music*	handle;
			};
			//SE
			template <size_t N>
			struct holder_se :holder_base {
				static_assert(!(N < IMATHLIB_INTERFACESE_NAM_), "The size of N is too large.");
				holder_se(const char* str) :channel(-1) {
					if (!(handle = Mix_LoadWAV(str)))
						controler::inst()->log_wrrite(log_output::error, Mix_GetError(), log_output::file_read_error, str);
				}
				virtual ~holder_se() { free(); }

				virtual	void play(size_t loop, size_t ms) {
					channel = Mix_GroupAvailable(AUDIO_TYPE_SYSTEM_SE);						//空いているチャンネルを探す
					if (channel == -1) channel = Mix_GroupNewer(AUDIO_TYPE_SYSTEM_SE);		//なければ一番古いチャンネル
					Mix_FadeInChannel(channel, handle, loop, ms);							//再生
				}
				virtual	void pause() { Mix_Pause(channel); }
				virtual void resume() { Mix_Resume(channel); }
				virtual	void reset(size_t ms) { Mix_FadeOutChannel(channel, ms); }

				virtual void free() {
					Mix_FreeChunk(handle);
					handle = nullptr;
				}
			private:
				Mix_Chunk*	handle;				//音楽のハンドル(Wave限定)
				int			channel;			//再生中のチャンネル
			};

			std::shared_ptr<holder_base> ptr;			//シェアポインターにするか考える

			//オーバーロードによるホルダーの作成
			template <size_t N>
			static std::shared_ptr<holder_base> allocate_holder(const char* str) {
				return std::shared_ptr<holder_base>(new holder_se<N - 1>(str));
			}
			template <>
			static std::shared_ptr<holder_base> allocate_holder<0>(const char* str) {
				return std::shared_ptr<holder_base>(new holder_bgm(str));
			}
			//ホルダーのデリート
			void deallocate_holder() {
				ptr.reset();
			}

		public:
			static const size_t bgm = 0;			//BGM識別
			static const size_t se = 1;				//SE識別(1～)

			template <size_t N>
			audio(const char* str, const audio_ident<N>& ident) :ptr(allocate_holder<N>(str)) {}
			~audio() { deallocate_holder(); }

			//オーディオの参照
			std::shared_ptr<holder_base>& operator()() { return ptr; }
			const std::shared_ptr<holder_base>& operator()() const { return ptr; }

			//コピー
			audio& operator=(const audio& a) {
				this->ptr = a.ptr;
				return *this;
			}
			audio& operator=(audio&& a) {
				this->ptr = a.ptr;
				return *this;
			}

			//音量の設定(百分率)
			static void set_volume_bgm(int v) {
				int volume_bmg = static_cast<int>((v / 100.0)*MIX_MAX_VOLUME);
				//ボリューム値の設定
				Mix_VolumeMusic(volume_bmg);
			}
			template <size_t N>
			static void set_volume_se(int v) {
				int volume_se = static_cast<int>((v / 100.0)*MIX_MAX_VOLUME);
				//ボリューム値の設定
				for (int i = 8; i <= 15; ++i)
					Mix_Volume(i, volume_se);
			}
			//全ての音楽の一時停止
			static void pause_all() {
				Mix_PauseMusic();
				Mix_Resume(-1);
			}
			//全ての音楽の一時停止を解除する
			static void resume_all() {
				Mix_ResumeMusic();
				Mix_Resume(-1);
			}
			//全ての音楽のリセット
			static void reset_all(size_t ms) {
				Mix_FadeOutMusic(ms);			//フェードアウトして音楽の停止
				Mix_RewindMusic();				//音楽を先頭に戻す
				Mix_FadeOutChannel(-1, ms);		//全てのチャネルをフェードアウトさせる
			}
		};
	}
}


#endif
#endif