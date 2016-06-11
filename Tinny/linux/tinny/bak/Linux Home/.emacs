;; ################################################################################################
;; General Settings
;; ################################################################################################


(custom-set-variables
  ;; custom-set-variables was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 '(case-fold-search t)
 '(column-number-mode t)
 '(delete-selection-mode t nil (delsel))
 '(global-font-lock-mode t nil (font-lock))
 '(line-number-mode t)
 '(scroll-bar-mode nil)
 '(show-paren-mode t nil (paren))
 '(tool-bar-mode nil nil (tool-bar))
 '(transient-mark-mode t))

;; emacs frame size
(setq default-frame-alist (append default-frame-alist '((top . 100) (left . 100) (width . 100) (height . 40) (cursor-type . bar))))

;; disable startup message
(setq inhibit-startup-message t)

;; emacs line space
(setq-default line-spacing 4)

;; make minibuffer larger if there is more to see
(resize-minibuffer-mode 1)

;; filename completion
(autoload 'comint-dynamic-complete-filename "comint" "Complete filenames." t)

;; shell mode customizations."
(add-hook 'shell-mode-hook 'n-shell-mode-hook)

;; buffer read-only
(setq buffer-read-only t)

;; emacs fill-column variable
(setq-default fill-column 80)

;; emacs comment-coumn variable
(setq-default comment-column 60)


;; ################################################################################################
;; Load Paths
;; ################################################################################################


;; load path
(add-to-list 'load-path "~/tinny/emacs/")


;; ################################################################################################
;; Color Settings
;; ################################################################################################


(custom-set-faces
  ;; custom-set-faces was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 '(default ((t (:background "black" :foreground "grey80" :weight normal :family "adobe-courier"))))
 '(cperl-array-face ((t (:foreground "orangered"))))
 '(cperl-hash-face ((t (:foreground "purple"))))
 '(font-lock-comment-face ((t (:foreground "red"))))
 '(font-lock-function-name-face ((t (:foreground "cyan"))))
 '(font-lock-keyword-face ((t (:foreground "gold"))))
 '(font-lock-string-face ((t (:foreground "green"))))
 '(font-lock-type-face ((t (:foreground "violet"))))
 '(font-lock-variable-name-face ((t (:foreground "yellow"))))
 '(font-lock-warning-face ((t (:foreground "violetred")))))


;; ################################################################################################
;; CEDET Settings
;; ################################################################################################


;; Load CEDET
;; (load "~/tinny/emacs/cedet-1.0pre4/common/cedet")

;; This enables some tools useful for coding, such as summary mode
;; imenu support, and the semantic navigator
;; (semantic-load-enable-code-helpers)

;; semanticdb-default-save-directory
;; (when (featurep 'semanticdb)
;;   (setq semanticdb-default-save-directory "~/tinny/emacs/emacs-semantic/")
;;   (when (not (file-exists-p semanticdb-default-save-directory))
;;     (make-directory semanticdb-default-save-directory)))


;; ################################################################################################
;; Subversion Settings
;; ################################################################################################


;; subversion
;; (require 'psvn)


;; ################################################################################################
;; Shell Script Settings
;; ################################################################################################


(add-to-list 'auto-mode-alist '("\\.bash$" . shell-script-mode))


;; ################################################################################################
;; Perl Settings
;; ################################################################################################


;; cperl mode
(add-to-list 'auto-mode-alist '("\\.pl$" . cperl-mode))
(add-to-list 'auto-mode-alist '("\\.pm$" . cperl-mode))
(add-to-list 'interpreter-mode-alist '("perl" . cperl-mode))
(setq cperl-close-paren-offset -4)
(setq cperl-continued-statement-offset 4)
(setq cperl-indent-level 4)
(setq cperl-indent-parens-as-block t)
(setq cperl-tab-always-indent t)
(setq cperl-comment-column 60)


;; ################################################################################################
;; C++ Settings
;; ################################################################################################


;; c++ mode
(add-to-list 'auto-mode-alist '("\\.cpp$" . c++-mode))
(add-to-list 'auto-mode-alist '("\\.h$" . c++-mode))

;; c++ mode hook
(setq c++-mode-hook
  (function (lambda ()
    (setq c-indent-level 4)
    (setq c-basic-offset 4)
    (setq indent-tabs-mode nil)
    (setq comment-column 60)
    (c-set-style "stroustrup")
    ;;(semantic-default-c-setup)
    )
  )
)


;; ################################################################################################
;; XML Settings
;; ################################################################################################


(load "~/tinny/emacs/nxml-mode-20041004/rng-auto")
(add-to-list 'auto-mode-alist '("\\.xml$" . nxml-mode))
(setq nxml-child-indent 4)
(setq nxml-outline-child-indent 4)


;; ################################################################################################
;; Smart Compilation
;; ################################################################################################


(autoload 'mode-compile      "mode-compile" t)
(autoload 'mode-compile-kill "mode-compile" t)


;; ################################################################################################
;; Key Bindings
;; ################################################################################################


;; key binding: delete forward
(global-set-key (kbd "<delete>") 'delete-char)                         ; [Delete]

;; key binding: undo
(global-set-key (kbd "C--") 'undo)	                               ; [Ctrl]-[-]

;; key binding: calls the shell
(global-set-key "\C-cd" 'shell)                                        ; [Ctrl]-[C] [D]

;; key binding: goto
(global-set-key "\C-cg" 'goto-line)	                               ; [Ctrl]-[C] [G]
(global-set-key "\C-cc" 'goto-char)                                    ; [Ctrl]-[C] [C]

;; key binding: filename completion in the normal buffer.
(global-set-key "\C-cf" 'comint-dynamic-complete-filename)             ; [Ctrl]-[C] [F]

;; scroller
(global-set-key (kbd "M-p") 'scroll-down-keep-cursor)                  ; [Alt]-[P]
(global-set-key (kbd "M-n") 'scroll-up-keep-cursor)                    ; [Alt]-[N]

;; key binding: smart completion
(global-set-key "\C-cl" ' semantic-ia-complete-symbol)                 ; [Ctrl]-[C] [L]

;; key binding: toggle truncate lines
(global-set-key (kbd "<f12>") 'toggle-truncate-lines)                  ; [f12]

;; key binding: switch to previous buffer
(global-set-key (kbd "<f3>") 'switch-to-previous-buffer)               ; [f3]

;; key binding: switch to next buffer
(global-set-key (kbd "<f4>") 'switch-to-next-buffer)                   ; [f4]

;; key binding: smart compillation
(global-set-key (kbd "<f7>") 'mode-compile)                            ; [f7]


;; ################################################################################################
;; Defined Functions
;; ################################################################################################


;; toggle truncate
(defun toggle-truncate-lines ()
  (interactive)
  (if (eq truncate-lines 't)
      (set-variable 'truncate-lines nil)
    (set-variable 'truncate-lines 't)
    )
  )

;; clear command
(defun n-shell-mode-hook ()
  (setq comint-input-sender 'n-shell-simple-send)
  )

(defun n-shell-simple-send (proc command)
  (cond
   ((string-match "^[ \t]*clear[ \t]*$" command)
    (comint-send-string proc "\n")
    (erase-buffer))
   (t (comint-simple-send proc command))
   )
  )

;; scroll down
(defun scroll-down-keep-cursor ()
  (interactive)
  (scroll-down 1)
  )

;; scroll up
(defun scroll-up-keep-cursor ()
  (interactive)
  (scroll-up 1)
  )

;; switch back
(defun switch-to-previous-buffer ()
  (interactive)
  (switch-to-buffer (nth (- (length (buffer-list)) 1) (buffer-list)))
  )

;; switch forward
(defalias 'switch-to-next-buffer 'bury-buffer)


;; ################################################################################################
;; Done
;; ################################################################################################
