#include <golos/plugins/account_history/plugin.hpp>
#include <golos/chain/operation_notification.hpp>
#include <golos/plugins/chain/plugin.hpp>
#include <golos/chain/database.hpp>
#include <fc/io/json.hpp>
#include <golos/chain/objects/history_object.hpp>

namespace golos {
    namespace plugins {
        namespace account_history {
            using namespace golos::chain;

            // TODO:  Review all of these, especially no-ops
            struct get_impacted_account_visitor {
                flat_set<account_name_type> &_impacted;

                get_impacted_account_visitor(flat_set<account_name_type> &impact) : _impacted(impact) {
                }

                typedef void result_type;

                template<typename T>
                void operator()(const T &op) {
                    op.get_required_posting_authorities(_impacted);
                    op.get_required_active_authorities(_impacted);
                    op.get_required_owner_authorities(_impacted);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_create_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.new_account_name);
                    _impacted.insert(op.creator);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_update_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const comment_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.author);
                    if (op.parent_author.size()) {
                        _impacted.insert(op.parent_author);
                    }
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const delete_comment_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.author);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const vote_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.voter);
                    _impacted.insert(op.author);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const author_reward_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.author);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const curation_reward_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.curator);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const liquidity_reward_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const interest_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const fill_convert_request_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_to_vesting_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);

                    if (op.to != account_name_type() && op.to != op.from) {
                        _impacted.insert(op.to);
                    }
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const withdraw_vesting_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const witness_update_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_witness_vote_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                    _impacted.insert(op.witness);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_witness_proxy_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                    _impacted.insert(op.proxy);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const feed_publish_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.publisher);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const limit_order_create_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const fill_order_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.current_owner);
                    _impacted.insert(op.open_owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const fill_call_order_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const fill_settlement_order_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const limit_order_cancel_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const pow_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.worker_account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const fill_vesting_withdraw_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from_account);
                    _impacted.insert(op.to_account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const shutdown_witness_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.owner);
                }

                void operator()(const custom_operation &op) {
                    for (auto s: op.required_auths) {
                        _impacted.insert(s);
                    }
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const request_account_recovery_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account_to_recover);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const recover_account_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account_to_recover);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const change_recovery_account_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account_to_recover);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_transfer_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                    _impacted.insert(op.agent);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_approve_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                    _impacted.insert(op.agent);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_dispute_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                    _impacted.insert(op.agent);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_release_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                    _impacted.insert(op.agent);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_to_savings_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_from_savings_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                    _impacted.insert(op.to);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const cancel_transfer_from_savings_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.from);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const decline_voting_rights_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const comment_benefactor_reward_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.benefactor);
                    _impacted.insert(op.author);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const delegate_vesting_shares_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.delegator);
                    _impacted.insert(op.delegatee);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const return_vesting_delegation_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const asset_update_operation<Major, Hardfork, Release> &op) {
                    if (op.new_issuer) {
                        _impacted.insert(*(op.new_issuer));
                    }
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const asset_issue_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.issue_to_account);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const override_transfer_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.to);
                    _impacted.insert(op.from);
                    _impacted.insert(op.issuer);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const call_order_update_operation<Major, Hardfork, Release> &op) {
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const bid_collateral_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.bidder);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const execute_bid_operation<Major, Hardfork, Release> &op) {
                    _impacted.insert(op.bidder);
                }

                //void operator()( const operation& op ){}
            };

            void operation_get_impacted_accounts(const operation &op, flat_set<account_name_type> &result) {
                get_impacted_account_visitor vtor = get_impacted_account_visitor(result);
                op.visit(vtor);
            }

            template<typename T>
            T dejsonify(const string &s) {
                return fc::json::from_string(s).as<T>();
            }

#define DEFAULT_VALUE_VECTOR(value) default_value({fc::json::to_string(value)}, fc::json::to_string(value))
#define LOAD_VALUE_SET(options, name, container, type) \
            if( options.count(name) ) { \
                  const std::vector<std::string>& ops = options[name].as<std::vector<std::string>>(); \
                  std::transform(ops.begin(), ops.end(), std::inserter(container, container.end()), &dejsonify<type>); \
            }

            struct operation_visitor {

                operation_visitor(database &db, const operation_notification &note, const operation_object *&n,
                                  string i) : _db(db), _note(note), new_obj(n), item(i) {
                };

                typedef void result_type;

                database &_db;
                const operation_notification &_note;
                const operation_object *&new_obj;
                string item;

                /// ignore these ops
                /*
                */


                template<typename Op>
                void operator()(Op &&) const {
                    const auto &hist_idx = _db.get_index<account_history_index>().indices().get<by_account>();
                    if (!new_obj) {
                        new_obj = &_db.create<operation_object>([&](operation_object &obj) {
                            obj.trx_id = _note.trx_id;
                            obj.block = _note.block;
                            obj.trx_in_block = _note.trx_in_block;
                            obj.op_in_trx = _note.op_in_trx;
                            obj.virtual_op = _note.virtual_op;
                            obj.timestamp = _db.head_block_time();
                            //fc::raw::pack( obj.serialized_op , _note.op);  //call to 'pack' is ambiguous
                            auto size = fc::raw::pack_size(_note.op);
                            obj.serialized_op.resize(size);
                            fc::datastream<char *> ds(obj.serialized_op.data(), size);
                            fc::raw::pack(ds, _note.op);
                        });
                    }

                    auto hist_itr = hist_idx.lower_bound(boost::make_tuple(item, uint32_t(-1)));
                    uint32_t sequence = 0;
                    if (hist_itr != hist_idx.end() && hist_itr->account == item) {
                        sequence = hist_itr->sequence + 1;
                    }

                    _db.create<account_history_object>([&](account_history_object &ahist) {
                        ahist.account = item;
                        ahist.sequence = sequence;
                        ahist.op = new_obj->id;
                    });
                }
            };

            struct operation_visitor_filter : operation_visitor {
                operation_visitor_filter(database &db, const operation_notification &note, const operation_object *&n,
                                         string i) : operation_visitor(db, note, n, i) {
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_to_vesting_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_create_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const account_update_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_to_savings_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const transfer_from_savings_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const cancel_transfer_from_savings_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_transfer_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_dispute_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_release_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<uint8_t Major, uint8_t Hardfork, uint16_t Release>
                void operator()(const escrow_approve_operation<Major, Hardfork, Release> &op) const {
                    operation_visitor::operator()(op);
                }

                template<typename Op>
                void operator()(Op &&op) const {
                }
            };

            using golos::chain::database;
            using namespace golos::protocol;
            using std::string;

            struct plugin::plugin_impl final {
            public:
                plugin_impl() : _self(appbase::app().get_plugin<chain::plugin>().db()) {
                }

                ~plugin_impl() {
                }

                golos::chain::database &database() {
                    return _self;
                }

                void on_operation(const operation_notification &note) {
                    flat_set<account_name_type> impacted;
                    golos::chain::database &db = database();

                    const operation_object *new_obj = nullptr;
                    operation_get_impacted_accounts(note.op, impacted);

                    for (const auto &item : impacted) {
                        auto itr = _tracked_accounts.lower_bound(item);
                        if (!_tracked_accounts.size() ||
                            (itr != _tracked_accounts.end() && itr->first <= item && item <= itr->second)) {
                            if (_filter_content) {
                                note.op.visit(operation_visitor_filter(db, note, new_obj, item));
                            } else {
                                note.op.visit(operation_visitor(db, note, new_obj, item));
                            }
                        }
                    }
                }

                golos::chain::database &_self;
                flat_map<string, string> _tracked_accounts;
                bool _filter_content = false;
            };


            plugin::plugin() : my(new plugin_impl) {
                ilog("Loading account history plugin");
            }


            plugin::~plugin() {
            }


            void plugin::set_program_options(boost::program_options::options_description &cli,
                                             boost::program_options::options_description &cfg) {
                cli.add_options()("track-account-range",
                                  boost::program_options::value<std::vector<std::string>>()->composing()->multitoken(),
                                  "Defines a range of accounts to track as a json pair [\"from\",\"to\"] [from,to]")(
                        "filter-posting-ops", "Ignore posting operations, only track transfers and account updates");
                cfg.add(cli);
            }

            void plugin::plugin_initialize(const boost::program_options::variables_map &options) {
                //ilog("Intializing account history plugin" );
                my->database().pre_apply_operation.connect([&](const operation_notification &note) {
                    my->on_operation(note);
                });

                using pairstring = pair<string, string>;
                LOAD_VALUE_SET(options, "track-account-range", my->_tracked_accounts, pairstring);
                if (options.count("filter-posting-ops")) {
                    my->_filter_content = true;
                }
            }

            void plugin::plugin_startup() {
                ilog("account_history plugin: plugin_startup() begin");

                ilog("account_history plugin: plugin_startup() end");
            }

            flat_map<string, string> plugin::tracked_accounts() const {
                return my->_tracked_accounts;
            }

        }
    }
}