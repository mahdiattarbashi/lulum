/*  filename: Protocol.h    2008/12/26  */
/*************************************************************************
    LuLu Messenger: A LAN Instant Messenger For Chatting and File Exchanging.
    Copyright (C) 2008,2009  Wu Weisheng <wwssir@gmail.com>

    This file is part of LuLu Messenger.

    LuLu Messenger is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LuLu Messenger is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/
/*************************************************************************
    LuLu 信使: 局域网络即时对话与文件交换通讯工具。
    著作权所有 (C) 2008,2009  武维生 <wwssir@gmail.com>

    此文件是 LuLu 信使源文件的一部分。

    LuLu 信使为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
    对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）
    任一日后发行的版本。

    LuLu 信使是基于使用目的而加以发布，然而不负任何担保责任；亦无对适售性或
    特定目的适用性所为的默示性担保。详情请参照GNU通用公共授权。

    您应已收到附随于本程序的GNU通用公共授权的副本；如果没有，请参照
    <http://www.gnu.org/licenses/>.
*************************************************************************/

// 说明：    
// 修改：    

#ifndef __Protocol_h__
#define __Protocol_h__

#include <ace/INET_Addr.h>
#include <ace/CDR_Stream.h>
#include <ace/CDR_Size.h>
#include <ace/Auto_Ptr.h>
#include <ace/Bound_Ptr.h>
#include <ace/Null_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <ace/Condition_T.h>
#include <ace/Synch_Traits.h>
#include <ace/SString.h>
#include <ace/Vector_T.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Date_Time.h>

namespace IMProto
{
        extern const char* Version;
        // Basic types.
        //
        typedef ACE_CDR::Boolean        boolean;
        typedef ACE_CDR::Octet          u8;
        typedef ACE_CDR::UShort         u16;
        typedef ACE_CDR::ULong          u32;
        typedef ACE_CDR::ULongLong      u64;

        typedef ACE_OutputCDR ostreamT;
        typedef ACE_SizeCDR sstreamT;
        typedef ACE_InputCDR istreamT;

        typedef ACE_SYNCH_MUTEX Mutex;
        typedef ACE_Guard<Mutex> Lock;
        typedef ACE_Condition<Mutex> Condition;

        typedef ACE_INET_Addr Address;

        //
        //
        //
        struct Profile;

        typedef ACE_Strong_Bound_Ptr<Profile, Mutex> Profile_ptr;

        struct Profile
        {
        public:
                class Header
                {
                public:
                        Header(u16 id, u16 size)
                                : id_(id), size_(size)
                        {
                        }

                        Header(istreamT& is)
                        {
                                (void)(is >> id_ >> size_);
                        }

                public:
                        u16 id() const
                        {
                                return id_;
                        }

                        u16 size() const
                        {
                                return size_;
                        }

                protected:
                        void size(u16 s)
                        {
                                size_ = s;
                        }

                        friend struct Profile;

                private:
                        u16 id_;
                        u16 size_;
                };

        public:
                virtual ~Profile()
                {
                }

                Profile_ptr clone()
                {
                        Profile_ptr p(clone_());
                        return p;
                }

        protected:
                Profile(u16 id)
                        : header_(id, 0)
                {
                }

                Profile(Header const& h)
                        : header_(h)
                {
                }

                virtual Profile_ptr clone_() = 0;

        private:
                Profile& operator=(Profile const&);

        public:
                u16 id() const
                {
                        return header_.id();
                }

                u16 size() const
                {
                        return header_.size();
                }

        protected:
                void size(u16 s)
                {
                        header_.size(s);
                }

                u16 calculate_size()
                {
                        sstreamT ss;

                        serialize_body(ss);

                        return static_cast<u16>(ss.total_length());
                }

        public:
                virtual void serialize_body(ostreamT&) const = 0;

                virtual void serialize_body(sstreamT&) const = 0;

                friend ostreamT& operator<<(ostreamT& os, Profile const& p);

                friend sstreamT& operator<<(sstreamT& ss, Profile const& p);

        private:
                Header header_;
        };

        inline ostreamT& operator<<(ostreamT& os, Profile::Header const& hdr)
        {
                os << hdr.id();
                os << hdr.size();

                return os;
        }

        inline sstreamT& operator<<(sstreamT& ss, Profile::Header const& hdr)
        {
                ss << hdr.id();
                ss << hdr.size();

                return ss;
        }

        inline ostreamT& operator<<(ostreamT& os, Profile const& p)
        {
                os << p.header_;
                p.serialize_body(os);

                return os;
        }

        inline sstreamT& operator<<(sstreamT& ss, Profile const& p)
        {
                ss << p.header_;
                p.serialize_body(ss);

                return ss;
        }

        //
        //
        //
        struct SN;

        typedef ACE_Strong_Bound_Ptr<SN, Mutex>
                SN_ptr;

        struct SN : Profile
        {
                static const u16 id;

        public:
                SN(Header const& h, istreamT& is)
                        : Profile(h)
                {
                        is >> n_;
                }

                SN(u64 n)
                        : Profile(id), n_(n)
                {
                        size(calculate_size());
                }

                SN_ptr clone()
                {
                        return SN_ptr(clone_());
                }

        protected:
                virtual Profile_ptr
                        clone_()
                {
                        Profile_ptr p(new SN(*this));
                        return p;
                }

                SN(SN const& sn)
                        : Profile(sn)
                        , n_(sn.n_)
                {
                }

        public:
                u64 num() const
                {
                        return n_;
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        os << n_;
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        ss << n_;
                }

        private:
                u64 n_;
        };

        //
        //
        //
        struct Ack;

        typedef ACE_Strong_Bound_Ptr<Ack, Mutex>
                Ack_ptr;

        struct Ack : Profile
        {
                static const u16 id;

        public:
                Ack(Header const& h, istreamT& is)
                        : Profile(h)
                {
                        is >> n_;
                }

                Ack(u64 n)
                        : Profile(id), n_(n)
                {
                        size(calculate_size());
                }

                Ack_ptr clone()
                {
                        return Ack_ptr(clone_());
                }

        protected:
                virtual Profile_ptr
                        clone_()
                {
                        Profile_ptr p(new Ack(*this));
                        return p;
                }

                Ack(Ack const& sn)
                        : Profile(sn)
                        , n_(sn.n_)
                {
                }

        public:
                u64 num() const
                {
                        return n_;
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        os << n_;
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        ss << n_;
                }

        private:
                u64 n_;
        };

        //
        //
        //
        class Message;

        typedef ACE_Strong_Bound_Ptr<Message, Mutex> Message_ptr;

        class Message
        {
                static u64 sn;
                typedef ACE_Hash_Map_Manager<u16, Profile_ptr, ACE_Null_Mutex>
                        Profiles;
        public:
                Message(void)
                        : profiles_(4)
                {
                        add(Profile_ptr(new SN(sn++)));
                }

                Message_ptr clone()
                {
                        Message_ptr cloned(new Message(*this));
                        return cloned;
                }

        protected:
                Message(Message const& m)
                        : profiles_(4)
                {
                        for(Profiles::const_iterator i(m.profiles_); !i.done(); i.advance())
                        {
                                // Shallow copy of profiles. This implies that profiles are not
                                // modified as they go up/down the stack.
                                //
                                profiles_.bind((*i).ext_id_,(*i).int_id_);
                        }
                }

        private:
                Message& operator=(Message const&);

        public:
                bool add(Profile_ptr p)
                {
                        u16 id(p->id());

                        if(profiles_.find(id) == 0)
                        {
                                return false;
                        }

                        profiles_.bind(id, p);

                        return true;
                }

                void replace(Profile_ptr p)
                {
                        profiles_.rebind(p->id(), p);
                }

                void remove(u16 id)
                {
                        profiles_.unbind(id);
                }

                Profile const* find(u16 id) const
                {
                        Profiles::ENTRY* e = 0;

                        if(profiles_.find(id, e) == -1) return 0;

                        return e->int_id_.get();
                }

                typedef Profiles::const_iterator
                        ProfileIterator;

                ProfileIterator begin() const
                {
                        return ProfileIterator(profiles_);
                }

        public:
                size_t size() const
                {
                        sstreamT ss;

                        u32 s(0);

                        ss << s;

                        for(Profiles::const_iterator i(profiles_); !i.done(); i.advance())
                        {
                                ss << *((*i).int_id_);
                        }

                        return ss.total_length();
                }

                friend ostreamT& operator<<(ostreamT& os, Message const& m)
                {
                        u32 s(m.size());

                        os << s;

                        for(Profiles::const_iterator i(m.profiles_); !i.done(); i.advance())
                        {
                                os << *((*i).int_id_);
                        }

                        return os;
                }
        private:
                Profiles profiles_;
        };

        typedef ACE_Vector<Message_ptr, ACE_VECTOR_DEFAULT_SIZE> Messages;

        //
        //
        //
        struct From;

        typedef
                ACE_Strong_Bound_Ptr<From, Mutex>
                From_ptr;

        struct From : Profile
        {
                static const u16 id;

        public:
                From(Header const& h, istreamT& is)
                        : Profile(h)
                {
                        u32 addr;
                        u16 port;

                        is >> addr;
                        is >> port;

                        address_ = Address(port, addr);
                }

                From(Address const& addr)
                        : Profile(id), address_(addr)
                {
                        size(calculate_size());
                }

                From_ptr clone()
                {
                        return From_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new From(*this));
                        return p;
                }

                From(From const& from)
                        : Profile(from), address_(from.address_)
                {
                }

        public:
                Address const& address() const
                {
                        return address_;
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        u32 addr(address_.get_ip_address());
                        u16 port(address_.get_port_number());

                        os << addr;
                        os << port;
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        u32 addr(0);
                        u16 port(0);

                        ss << addr;
                        ss << port;
                }

        private:
                Address address_;
        };


        //
        //
        //
        struct To;

        typedef ACE_Strong_Bound_Ptr<To, Mutex>
                To_ptr;

        struct To : Profile
        {
                static const u16 id;

        public:
                To(Header const& h, istreamT& is)
                        : Profile(h)
                {
                        u32 addr;
                        u16 port;

                        is >> addr;
                        is >> port;

                        address_ = Address(port, addr);
                }

                To(Address const& addr)
                        : Profile(id), address_(addr)
                {
                        size(calculate_size());
                }

                To_ptr clone()
                {
                        return To_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new To(*this));
                        return p;
                }

                To(To const& to)
                        : Profile(to), address_(to.address_)
                {
                }

        public:
                Address const& address() const
                {
                        return address_;
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        u32 addr(address_.get_ip_address());
                        u16 port(address_.get_port_number());

                        os << addr;
                        os << port;
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        u32 addr(0);
                        u16 port(0);

                        ss << addr;
                        ss << port;
                }

        private:
                Address address_;
        };

        //////////////////////////////////////////////////////////////////////////
        enum Id
        {
                IdNull = 0x0000,
                FromId = 0x0001,
                ToId,
                SNId,
                AckId,
                DataId,

                SignUpId = 0x0020,
                SignInId,
                SignOutId,
                FacthPropertyId,
                UpdatePropertyId,

                TextNormal = 0x0040,
                TextZip,

                FileSend = 0x0060, 
                DirSend, 
                DirFileRecv,
                DirFileSendCancel,
                DirFileRecvCancel,
                PictureSend, 
                PictureRecv,
                SharedList, 
                SharedListAck, 
                SharedUploadCancel, 
                SharedDownload,
                SharedDownloadCancel, 

                IdEnd
        };

        enum PropertyId
        {
                PIdStart = 0x0000,
                MsgFrom,        // 保留
                MsgTo,          // 保留
                MsgSn,          // 保留
                TimeStamp,
                ProtocolName,   // 保留
                ProtocolVer,
                ProfileVer,     // 保留
                ProgramVer,

                MacAddress,
                IpAddress,
                ComputerId,     // 保留
                ComputerUserName,// 保留
                HostName,
                UserName,
                SignName,
                IconName,
                Password,
                OsName,
                OsVer,          // 保留，通过OsName传递

                TextContent,
                TextLength,
                TextZipLen,

                ListenPort,     // 文件传输事件中监听TCP连接一方的监听端口
                EventSn,        // 在某一用户中唯一标识一个传输事件的数字字符串，不同的用户可能有相同的Sn
                FileKey,        // 软件中唯一标识一个文件的字符串，一般为文件名或文件名加序号
                FileSize,
                FileName,

                PIdEnd
        };

        enum UserState
        {
                Available,
                Busy,
                Left,
                Inputing
        };

        // properties type
        typedef ACE_Hash_Map_Manager<u16, ACE_TString, ACE_Null_Mutex>
                Properties;

        //
        //
        //
        struct ProfileWithProperties;
        typedef ACE_Strong_Bound_Ptr<ProfileWithProperties, Mutex>
                ProfileWithProperties_ptr;

        struct ProfileWithProperties : Profile
        {
        public:
                ProfileWithProperties(Header const& h, istreamT& is)
                        : Profile(h)
                        , properties_(4)
                {
                        istreamT nis(is.rd_ptr(), h.size(), 1);
                        u16 cnt(0);
                        nis >> cnt;
                        for(int i = 0; i < cnt; i++)
                        {
                                u16 id;
                                ACE_TString p;
                                nis >> id;
                                nis >> p;
                                this->add(id, p.c_str());
                        }

                        is.skip_bytes(h.size());
                }

                ProfileWithProperties(u16 id)
                        : Profile(id)
                        , properties_(4)
                {
                        add_timeStamp();
                }

                ProfileWithProperties(u16 id, Properties const& propers)
                        : Profile(id)
                        , properties_(propers.current_size())
                {
                        for(Properties::const_iterator i(propers.begin()); i != propers.end(); i++)
                        {
                                this->add((*i).ext_id_,(*i).int_id_.c_str(), false);
                        }
                        add_timeStamp();
                        size(calculate_size());
                }

                ProfileWithProperties_ptr clone()
                {
                        return ProfileWithProperties_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new ProfileWithProperties(*this));
                        return p;
                }

                ProfileWithProperties(ProfileWithProperties const& t)
                        : Profile(t)
                        //                         , properties_(t.properties_)
                {
                        for(Properties::const_iterator i(t.properties_.begin()); i != t.properties_.end(); i++)
                        {
                                this->add((*i).ext_id_,(*i).int_id_.c_str());
                        }
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        u16 cnt(properties_.current_size());
                        os << cnt;
                        for(Properties::const_iterator i(properties_.begin()); i != properties_.end(); i++)
                        {
                                os <<(*i).ext_id_;
                                os <<(*i).int_id_;
                        }
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        u16 cnt(0);
                        ss << cnt;
                        for(Properties::const_iterator i(properties_.begin()); i != properties_.end(); i++)
                        {
                                ss <<(*i).ext_id_;
                                ss <<(*i).int_id_;
                        }
                }

                // properties operations
        public:
                bool add(u16 id, const char* proper, bool update_siz = true)
                {
                        Properties::ENTRY* e = 0;
                        if(properties_.find(id, e) == 0)
                        {
                                return false;
                        }
                        if(properties_.bind(id, proper) == -1)
                        {
                                return false;
                        }
                        if (update_siz)
                        {
	                        size(calculate_size());
                        }
                        return true;
                }

                bool replace(u16 id, const char* p, bool update_siz = true)
                {
                        if(properties_.rebind(id, p) == -1)
                        {
                                return false;
                        }
                        if (update_siz)
                        {
                                size(calculate_size());
                        }
                        return true;
                }

                bool remove(u16 id, bool update_siz = true)
                {
                        if(properties_.unbind(id) == -1)
                        {
                                return false;
                        }
                        if (update_siz)
                        {
                                size(calculate_size());
                        }
                        return true;
                }

                void update_size()
                {
                        size(calculate_size());
                }

                const char* property_data(u16 id) const
                {
                        Properties::ENTRY* e = 0;
                        if(properties_.find(id, e) == -1) return 0;
                        return e->int_id_.c_str();
                }

                bool has_property(u16 id, Properties::ENTRY* e = 0)
                {
                        if (e)
                        {
                                if(properties_.find(id, e) == -1) return false;
                                return true;
                        } 
                        else
                        {
                                if(properties_.find(id) == -1) return false;
                                return true;
                        }
                }

                void add_timeStamp()
                {
                        ACE_Date_Time now;
                        now.update();
                        char ti[100];
#ifdef QT_NO_DEBUG
                        ACE_OS::sprintf(ti, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
                                now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
#else
                        sprintf(ti, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
                                now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
#endif
                        add(TimeStamp, ti);
                }

        protected:
                Properties properties_;
        };

        typedef ProfileWithProperties PFacthProperty;
        typedef ProfileWithProperties PUpdateProperty;

        typedef ProfileWithProperties PTextNormal;
        typedef ProfileWithProperties PTextZip;

        typedef ProfileWithProperties PFileSend;
        typedef ProfileWithProperties PDirSend;
        typedef ProfileWithProperties PDirFileRecv;
        typedef ProfileWithProperties PDirFileSendCancel;
        typedef ProfileWithProperties PDirFileRecvCancel;

        typedef ProfileWithProperties PPictureSend;
        typedef ProfileWithProperties PPictureRecv;

        typedef ProfileWithProperties PSharedList;
        typedef ProfileWithProperties PSharedDownload;
        typedef ProfileWithProperties PSharedDownloadCancel;
        typedef ProfileWithProperties PSharedUploadCancel;

        //
        //
        //
        struct FacthProperty;
        typedef ACE_Strong_Bound_Ptr<FacthProperty, Mutex>
                FacthProperty_ptr;

        struct FacthProperty : ProfileWithProperties
        {
                static const u16 id;

        public:
                FacthProperty(Header const& h, istreamT& is)
                        : ProfileWithProperties(h, is)
                {
                }

                FacthProperty(Properties const& args)
                        : ProfileWithProperties(id, args)
                {
                }

                FacthProperty(void)
                        : ProfileWithProperties(id)
                {
                }

                FacthProperty_ptr clone()
                {
                        return FacthProperty_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new FacthProperty(*this));
                        return p;
                }

                FacthProperty(FacthProperty const& t)
                        : ProfileWithProperties(t)
                {
                }
        };

        //
        //
        //
        struct UpdateProperty;
        typedef ACE_Strong_Bound_Ptr<UpdateProperty, Mutex>
                UpdateProperty_ptr;

        struct UpdateProperty : ProfileWithProperties
        {
                static const u16 id;

        public:
                UpdateProperty(Header const& h, istreamT& is)
                        : ProfileWithProperties(h, is)
                {
                }

                UpdateProperty(Properties const& args)
                        : ProfileWithProperties(id, args)
                {
                }

                UpdateProperty(void)
                        : ProfileWithProperties(id)
                {
                }

                UpdateProperty_ptr clone()
                {
                        return UpdateProperty_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new UpdateProperty(*this));
                        return p;
                }

                UpdateProperty(UpdateProperty const& t)
                        : ProfileWithProperties(t)
                {
                }
        };

        //
        //
        //
        struct SignUp;
        typedef ACE_Strong_Bound_Ptr<SignUp, Mutex>
                SignUp_ptr;

        struct SignUp : ProfileWithProperties
        {
                static const u16 id;

        public:
                SignUp(Header const& h, istreamT& is)
                        : ProfileWithProperties(h, is)
                {
                }

                SignUp(Properties const& args)
                        : ProfileWithProperties(id, args)
                {
                }

                SignUp(void)
                        : ProfileWithProperties(id)
                {
                }

                SignUp_ptr clone()
                {
                        return SignUp_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new SignUp(*this));
                        return p;
                }

                SignUp(SignUp const& t)
                        : ProfileWithProperties(t)
                {
                }
        };

        //
        //
        //
        struct SignIn;
        typedef ACE_Strong_Bound_Ptr<SignIn, Mutex>
                SignIn_ptr;

        struct SignIn : ProfileWithProperties
        {
                static const u16 id;

        public:
                SignIn(Header const& h, istreamT& is)
                        : ProfileWithProperties(h, is)
                {
                }

                SignIn(Properties const& args)
                        : ProfileWithProperties(id, args)
                {
                }

                SignIn(void)
                        : ProfileWithProperties(id)
                {
                }

                SignIn_ptr clone()
                {
                        return SignIn_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new SignIn(*this));
                        return p;
                }

                SignIn(SignIn const& t)
                        : ProfileWithProperties(t)
                {
                }
        };

        //
        //
        //
        struct SignOut;
        typedef ACE_Strong_Bound_Ptr<SignOut, Mutex>
                SignOut_ptr;

        struct SignOut : ProfileWithProperties
        {
                static const u16 id;

        public:
                SignOut(Header const& h, istreamT& is)
                        : ProfileWithProperties(h, is)
                {
                }

                SignOut(Properties const& args)
                        : ProfileWithProperties(id, args)
                {
                }

                SignOut(void)
                        : ProfileWithProperties(id)
                {
                }

                SignOut_ptr clone()
                {
                        return SignOut_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new SignOut(*this));
                        return p;
                }

                SignOut(SignOut const& t)
                        : ProfileWithProperties(t)
                {
                }
        };

        //
        //
        //
        struct Data;

        typedef ACE_Strong_Bound_Ptr<Data, Mutex>
                Data_ptr;

        struct Data : Profile
        {
                static const u16 id;

        public:
                virtual ~Data()
                {
                        if(buf_)
                                operator delete(buf_);
                }

                Data(Header const& h, istreamT& is)
                        : Profile(h),
                        buf_(0),
                        size_(h.size()),
                        capacity_(size_)
                {
                        if(size_)
                        {
                                buf_ = reinterpret_cast<char*>(operator new(capacity_));
                                is.read_char_array(buf_, size_);
                        }
                }

                Data(void const* buf, size_t s, size_t capacity = 0)
                        : Profile(id),
                        buf_(0),
                        size_(s),
                        capacity_(capacity < size_ ? size_ : capacity)
                {
                        if(size_)
                        {
                                buf_ = reinterpret_cast<char*>(operator new(capacity_));
                                ACE_OS::memcpy(buf_, buf, size_);
                        }

                        Profile::size(calculate_size());
                }

                Data_ptr clone()
                {
                        return Data_ptr(clone_());
                }

        protected:
                virtual Profile_ptr clone_()
                {
                        Profile_ptr p(new Data(*this));
                        return p;
                }

                Data(Data const& d)
                        : Profile(d),
                        buf_(0),
                        size_(d.size_),
                        capacity_(d.capacity_)
                {
                        if(size_)
                        {
                                buf_ = reinterpret_cast<char*>(operator new(capacity_));
                                ACE_OS::memcpy(buf_, d.buf_, size_);
                        }

                        Profile::size(calculate_size());
                }

        public:
                char const*  buf() const
                {
                        return buf_;
                }

                char* buf()
                {
                        return buf_;
                }

                size_t size() const
                {
                        return size_;
                }

                void size(size_t s)
                {
                        if(s > capacity_)
                                ACE_OS::abort();

                        size_ = s;

                        Profile::size(calculate_size());
                }

                size_t capacity() const
                {
                        return capacity_;
                }

        public:
                virtual void serialize_body(ostreamT& os) const
                {
                        os.write_char_array(buf_, size_);
                }

                virtual void serialize_body(sstreamT& ss) const
                {
                        ss.write_char_array(buf_, size_);
                }

        private:
                char* buf_;
                size_t size_;
                size_t capacity_;
        };
}


#endif // __Protocol_h__

